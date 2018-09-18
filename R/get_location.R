#' Get Location for a Vector of lat/lon coordinates.
#' 
#' Takes a vector of lat/lon coordinates, or a list of lat/lon coordinates, 
#' sends them to the Baidu Maps API, output a char vector of addresses and 
#' address component info, as json text objects. This function also implements 
#' a data dictionary, saved as an rda object, to avoid sending the same query 
#' to Baidu twice.
#'
#' @param lat numeric vector, vector of latitude values.
#' @param lon numeric vector, vector of longitude values.
#' @param force logical, force online query, even if previously downloaded and 
#'   saved to the data dictionary.
#' @param cache_chunk_size integer, indicates how often you want the API return
#'   data to be saved to the package cache. Default value is NULL.
#'
#' @return char vector of json text objects. Each object contains the return 
#'   value(s) from the Baidu Maps query, as well as the return value status 
#'   code.
#' @export
#'
#' @examples \dontrun{
#' lat <- c(
#'   30.616167082550779, 
#'   30.39624844375698, 
#'   30.679942845419565
#' )
#' 
#' lon <- c(
#'   114.27287244473057, 
#'   119.87833669326516, 
#'   104.06792346330406
#' )
#' 
#' bmap_get_location(lat, lon)
#' }
bmap_get_location <- function(lat, lon, force = FALSE, cache_chunk_size = NULL) {
  # Input validation.
  stopifnot(is.numeric(lat))
  stopifnot(is.numeric(lon))
  stopifnot(is.logical(force))
  stopifnot(is.integer(cache_chunk_size) || is.null(cache_chunk_size))
  
  if (!identical(length(lat), length(lon))) {
    stop("length of 'lat' and 'lon' must match")
  }
  
  # Check to make sure key is not NULL.
  if (is.null(bmap_env$bmap_key)) {
    stop(missing_key_msg(), call. = FALSE)
  }
  
  # Load address cache data (if it's not already loaded).
  load_address_cache()
  
  # Record number of observations in addr_hash_map.
  cache_len <- length(bmap_env$addr_hash_map)
  
  # Iterate over "location". If obj exists in addr_hash_map, return 
  # its json object. Otherwise, perform Baidu query, write the result to the 
  # addr_hash_map, and return the result.
  out <- vector(length = length(lat), mode = "character")
  for (x in seq_len(length(lat))) {
    
    # If last api query was less than 1 seconds ago, delay code by 1 seconds.
    if (Sys.time() < timestamp_of_last_query() + 1) {
      Sys.sleep(1)
    }
    
    # Check to make sure we're not over the daily query limit.
    if (bmap_remaining_daily_queries() == 0) {
      # If any obs were added to addr_hash_map, write the changes to file.
      if (length(bmap_env$addr_hash_map) > cache_len) {
        update_cache_data(address_cache = TRUE)
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
    
    # Generate query uri.
    uri <- get_addr_query_uri(lon[x], lat[x])
    
    # If lon_x or lat_x is NA, return NA.
    if (any(is.na(c(lon[x], lat[x])))) {
      out[x] <- NA
    
    # elif lon/lat in addr_hash_map & force == FALSE, return json obj from 
    # addr_hash_map.
    } else if (!force && in_addr_hash_map(uri)) {
      out[x] <- bmap_env$addr_hash_map[[uri]]
    
    # else sent query to Baidu Maps API to get coordinates. Result will be 
    # returned as a json text obj, and saved to addr_hash_map.
    } else {
      # Time stamp the current api query.
      assign("time_of_last_query", Sys.time(), envir = bmap_env)
      
      # Perform API query.
      res <- baidu_location_query(uri)
      
      # Edit cache variable "queries_left_today" to be one less.
      assign("queries_left_today", bmap_remaining_daily_queries() - 1L, 
             envir = bmap_env)
      
      # If API key is invalid, throw error.
      if (grepl("message", res) && !grepl('\"lng\"', res)) {
        stop(invalid_key_msg(res), call. = FALSE)
      }
      
      # Assign res to output vector.
      out[x] <- res
      
      # If there was a connection error or http status code 302, do not cache 
      # the result to addr_hash_map.
      if (grepl('con error:|"status\\":302', res)) {
        next
      }
      
      # If force == TRUE and uri already exists in addr_hash_map, do not cache
      # the resutls to addr_hash_map.
      if (force && in_addr_hash_map(uri)) {
        next
      }
      
      # Cache result to addr_hash_map.
      insert_addr_hash_map(uri, res)
    }
    
    # If cache_chunk_size is not NULL, and current iteration is evenly dvisible
    # by cache_chunk_size, write current API data to the package cache.
    if (!is.null(cache_chunk_size) && x %% cache_chunk_size == 0) {
      if (length(bmap_env$addr_hash_map) > cache_len) {
        update_cache_data(address_cache = TRUE)
      }
    }
  }
  
  # If any obs were added to addr_hash_map, write the changes to file.
  if (length(bmap_env$addr_hash_map) > cache_len) {
    update_cache_data(address_cache = TRUE)
  }
  
  # Assign attributes to the output vector.
  if (!exists("out_msg", inherits = FALSE)) {
    out_msg <- "all queries completed"
  }
  attributes(out)$msg <- out_msg
  attributes(out)$daily_queries_remaining <- bmap_remaining_daily_queries()
  attributes(out)$key_used <- bmap_env$bmap_key
  return(out)
}


#' Send lat/lon query to Baidu Maps API, return full address of the lat/lon
#'
#' @param uri char string, uri to query
#'
#' @return json obj (as char string) containing the address API return data.
#' 
#' @noRd
baidu_location_query <- function(uri) {
  # Add API key to the query uri.
  uri <- sprintf(uri, get("bmap_key", envir = bmap_env))
  
  # Query
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
