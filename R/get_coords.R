#' Get Coordinates for a Vector of Locations.
#' 
#' Takes a vector of locations (address, business names, etc), sends them to 
#' the Baidu Maps API, output a char vector of coordinates, as json text 
#' objects. This function also implements a data dictionary, saved as an rda 
#' object, to avoid sending the same query to Baidu twice.
#'
#' @param location char vector, vector of locations.
#' @param type string, dictates the return data type. Valid inputs are 
#'   \code{data.frame}, which will return a data.frame, or \code{json}, which 
#'   will return a vector of json strings.
#' @param force logical, force online query, even if previously downloaded and 
#'   saved to the data dictionary.
#' @param skip_short_str logical, if TRUE then any input strings with length 
#'   of three or less will not be queried in the Baidu API, and a json str with 
#'   custom status code will be returned, indicating the str is too short to 
#'   query. The idea is that if a str is three char's or less, then it's 
#'   probably not an actual address, company/business name, or location. 
#'   Default value is FALSE.
#' @param cache_chunk_size integer, indicates how often you want the API return
#'   data to be saved to the package cache. Default value is NULL.
#'
#' @return char vector of json text objects. Each object contains the return 
#'   value(s) from the Baidu Maps query, as well as the return value status 
#'   code.
#' @export
#' 
#' @examples \dontrun{
#' locs <- c(
#'   "中百超市有限公司长堤街二分店", 
#'   "浙江省杭州市余杭区径山镇小古城村", 
#'   "成都高梁红餐饮管理有限公司"
#' )
#' 
#' bmap_get_coords(locs)
#' }
#' @useDynLib baidugeo, .registration = TRUE
#' @importFrom Rcpp sourceCpp
bmap_get_coords <- function(location, type = c("data.frame", "json"), 
                            force = FALSE, skip_short_str = FALSE, 
                            cache_chunk_size = NULL) {
  # Input validation.
  stopifnot(is.character(location))
  type <- match.arg(type)
  stopifnot(is.logical(force))
  stopifnot(is.logical(skip_short_str))
  stopifnot(is.integer(cache_chunk_size) || is.null(cache_chunk_size))
  
  # Check to make sure key is not NULL.
  if (is.null(bmap_env$bmap_key)) {
    stop(missing_key_msg(), call. = FALSE)
  }
  
  # Load coord cache data (if it's not already loaded).
  load_coord_cache()
  
  # Record number of observations in coord_hash_map.
  cache_len <- length(bmap_env$coord_hash_map)
  
  # Iterate over "location". If obj exists in coord_hash_map, return 
  # its json object. Otherwise, perform Baidu query, write the result to 
  # coord_hash_map, and return the result.
  out <- vector(length = length(location), mode = "character")
  for (x in seq_len(length(location))) {
    
    # If last api query was less than 1 seconds ago, delay code by 1 seconds.
    if (Sys.time() < timestamp_of_last_query() + 1) {
      Sys.sleep(1)
    }
    
    # Check to make sure we're not over the daily query limit.
    if (bmap_remaining_daily_queries() == 0) {
      # If any obs were added to coord_hash_map, write the changes to file.
      if (length(bmap_env$coord_hash_map) > cache_len) {
        update_cache_data(coordinate_cache = TRUE)
      }
      out_msg <- paste("rate limit has been reached for the day for key:", 
                       bmap_env$bmap_key)
      out <- out[1:(x - 1)]
      break
    }
    
    # Check to see if the 24 hour daily query limit timer needs to be reset.
    # If it does, then reset the daily numeric rate limit as well.
    if (is.null(get_limit_reset_time()) || 
        get_limit_reset_time() < Sys.time()) {
      limit_reset()
    }
    
    # Look up current location in coord_hash_map.
    curr_hash <- bmap_env$coord_hash_map[[digest::digest(location[x])]]
    
    # If the current location is NA or NULL, return "NA".
    if (is.null(location[x]) || is.na(location[x])) {
      out[x] <- NA
    
    # if x in saved_coords & force == FALSE, look up x in coord_hash_map 
    # and return json obj.
    } else if (!force && !is.null(curr_hash)) {
      out[x] <- curr_hash[2]
    
    # elif skip_short_str == TRUE & nchar(x) <= 3, return custom json obj.
    } else if (skip_short_str && nchar(location[x]) <= 3) {
      res <- paste0('{\"status\":6,\"msg\":\"len of str is 3 or', 
                    ' fewer chars\",\"results\":[]}')
      out[x] <- res
      insert_coord_hash_map(location[x], res)
    
    # else sent query to Baidu Maps API to get coordinates. Result will be 
    # returned as a json text obj, and saved to the data dictionary.
    } else {
      # Time stamp the current api query.
      assign("time_of_last_query", Sys.time(), envir = bmap_env)
      
      # Perform API query.
      res <- baidu_coord_query(location[x])
      
      # Edit cache variable "queries_left_today" to be one less.
      assign("queries_left_today", (bmap_remaining_daily_queries() - 1L), 
             envir = bmap_env)
      
      # If API key is invalid, throw error.
      if (grepl("message", res) && !grepl('\"lng\"', res)) {
        stop(invalid_key_msg(res), call. = FALSE)
      }
      
      # Assign res to output vector.
      out[x] <- res
      
      # If API key is invalid, throw error.
      if (grepl("message", res) && !grepl('\"lng\"', res)) {
        stop(invalid_key_msg(res), call. = FALSE)
      }
      
      # If there was a connection error or http status code 302, do not cache 
      # the result to addr_hash_map.
      if (grepl('con error:|"status\\":302', res)) {
        next
      }
      
      # If force == TRUE and uri already exists in addr_hash_map, do not cache
      # the resutls to addr_hash_map.
      if (force && !is.null(curr_hash)) {
        next
      }
      
      # Cache result to coord_hash_map.
      insert_coord_hash_map(location[x], res)
      
    }
    
    # If cache_chunk_size is not NULL, and current iteration is evenly dvisible
    # by cache_chunk_size, write current API data to the package cache.
    if (!is.null(cache_chunk_size) && x %% cache_chunk_size == 0) {
      if (length(bmap_env$coord_hash_map) > cache_len) {
        update_cache_data(coordinate_cache = TRUE)
      }
    }
  }
  
  # If any obs were added to coord_hash_map, write the changes to file.
  if (length(bmap_env$coord_hash_map) > cache_len) {
    update_cache_data(coordinate_cache = TRUE)
  }
  
  # If "out_msg" doesn't exist, create it.
  if (!exists("out_msg", inherits = FALSE)) {
    out_msg <- "all queries completed"
  }
  
  # If input arg "type" is data.frame, parse the vector of json strings, 
  # extract data into a data frame.
  if (type == "data.frame") {
    out <- from_json_coords_vector(location, out)
  }
  
  # Assign attributes to the output object.
  attributes(out)$msg <- out_msg
  attributes(out)$daily_queries_remaining <- bmap_remaining_daily_queries()
  attributes(out)$key_used <- bmap_env$bmap_key
  return(out)
}


#' Send address/location query to Baidu Maps API, return a lat/lon
#'
#' @param location char string, location name or address to query.
#'
#' @return json obj (as char string) containing the lat/lon return data.
#' @noRd
baidu_coord_query <- function(location) {
  # Check for presence of chars " " and "#", as either will produce errors 
  # if sent to the Baidu Maps API. Find and eliminate them.
  if (any(grepl(' |#', location))) {
    location <- gsub(' |#', '', location)
  }
  
  # Compile query URL.
  uri <- sprintf(
    get_coords_query_uri(location), get("bmap_key", envir = bmap_env)
  )
  
  # query
  res <- tryCatch(httr::GET(uri), error = function(e) e)
  
  # Check return for error, then return the value.
  if (methods::is(res, "error")) {
    res <- "con error: err"
  } else if (httr::status_code(res) != 200) {
    res <- paste("con error:", httr::status_code(res))
  } else {
    res <- httr::content(res, as = "text", encoding = "UTF-8")
  }
  
  return(res)
}
