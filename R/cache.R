
#' Addr Hash Map Lookup
#'
#' @noRd
in_addr_hash_map <- function(key) {
  exists(key, envir = bmap_env$addr_hash_map, inherits = FALSE)
}


#' Coord Hash Map Insert
#'
#' @noRd
insert_coord_hash_map <- function(key, value) {
  bmap_env$coord_hash_map[[digest::digest(key)]] <- c(key, value)
}


#' Addr Hash Map Insert
#'
#' @noRd
insert_addr_hash_map <- function(key, value) {
  bmap_env$addr_hash_map[[key]] <- value
}


#' Load Coordinate Cache
#'
#' @noRd
load_coord_cache <- function() {
  if (is.null(bmap_env$coord_hash_map)) {
    coord_hash_map <- system.file("extdata", 
                                  "coordinate_cache.rda", 
                                  package = "baidugeo")
    if (file.exists(coord_hash_map)) {
      load(coord_hash_map, envir = bmap_env)
    } else {
      warning("Cannot identify package data file 'coordinate_cache.rda'")
    }
  }
}


#' Load Address Cache
#'
#' @noRd
load_address_cache <- function() {
  if (is.null(bmap_env$addr_hash_map)) {
    addr_hash_map <- system.file("extdata", 
                                 "address_cache.rda", 
                                 package = "baidugeo")
    if (file.exists(addr_hash_map)) {
      load(addr_hash_map, envir = bmap_env)
    } else {
      warning("Cannot identify package data file 'address_cache.rda'")
    }
  }
}


#' Save updated cache data set to inst/extdata as package data.
#'
#' @noRd
update_cache_data <- function(coordinate_cache = FALSE, 
                              address_cache = FALSE) {
  if (coordinate_cache) {
    # Save coord_hash_map to file..
    save(
      coord_hash_map,
      file = paste0(system.file("extdata", package = "baidugeo"),
                    "/coordinate_cache.rda"),
      compress = "bzip2", 
      envir = bmap_env
    )
  }
  if (address_cache) {
    # Save addr_hash_map to file..
    save(
      addr_hash_map,
      file = paste0(system.file("extdata", package = "baidugeo"),
                    "/address_cache.rda"),
      compress = "bzip2", 
      envir = bmap_env
    )
  }
}


#' Clear Cached Data Files
#' 
#' This function gives the user the ability to clear one or both of the cached 
#' data sets on file. Doing so for either of the datasets will completely 
#' erase all data for the dataset specified. Do NOT run this function if you 
#' don't want to loose any of the cached data currently on file.
#'
#' @param coordinate_cache logical, if TRUE, all data currently in 
#'  coordinate_cache.rda will be cleared. Default value is FALSE.
#' @param address_cache logical, if TRUE, all data currently in 
#'  address_cache.rda will be cleared. Default value is FALSE.
#'
#' @return If both func args are set to FALSE, function will return NULL. 
#'  Otherwise, function does not return a value.
#' @export
#'
#' @examples \dontrun{
#' # Clear both cached data sets.
#' bmap_clear_cache(coordinate_cache = TRUE, address_cache = TRUE)
#' 
#' # Clear only the address cache.
#' bmap_clear_cache(address_cache = TRUE)
#' }
bmap_clear_cache <- function(coordinate_cache = FALSE, 
                             address_cache = FALSE) {
  stopifnot(is.logical(coordinate_cache))
  stopifnot(is.logical(address_cache))
  
  if (!coordinate_cache && !address_cache) {
    message(paste0("No cached datasets were cleared.\nSet function args to ", 
                   "TRUE in order to clear one or both of the cached ", 
                   "datasets."))
    return(NULL)
  }
  
  if (coordinate_cache && address_cache) {
    clear_coord_cache()
    clear_addr_cache()
    message("Both cached datasets were cleared.")
  } else if (coordinate_cache) {
    clear_coord_cache()
    message("The coordinates dataset was cleared.")
  } else if (address_cache) {
    clear_addr_cache()
    message("The addresses dataset was cleared.")
  } else {
    message(paste0("No cached datasets were cleared.\nSet function args to ", 
                   "TRUE in order to clear one or both of the cached ", 
                   "datasets."))
  }
}


#' Clear Coordinates Cached Data
#'
#' @noRd
clear_coord_cache <- function() {
  coord_hash_map <- new.env()
  save(
    coord_hash_map,
    file = paste0(system.file("extdata", package = "baidugeo"),
                  "/coordinate_cache.rda"),
    compress = "bzip2", 
    envir = bmap_env
  )
}


#' Clear Address Cache Data
#'
#' @noRd
clear_addr_cache <- function() {
  addr_hash_map <- new.env()
  save(
    addr_hash_map,
    file = paste0(system.file("extdata", package = "baidugeo"),
                  "/address_cache.rda"),
    compress = "bzip2", 
    envir = bmap_env
  )
}


#' Get Cached Coordinate Data
#' 
#' Return all cached coordinate data, as a tidy data frame.
#'
#' @return data frame
#' @export
#'
#' @examples \dontrun{
#' df <- bmap_get_cached_coord_data()
#' }
bmap_get_cached_coord_data <- function() {
  # Load the cache coordinates data set.
  load_coord_cache()
  cache_len <- length(bmap_env$coord_hash_map)
  keys <- names(bmap_env$coord_hash_map)
  
  # If length of the cached data set is zero, print message, then return NULL.
  if (cache_len == 0) {
    message("'coordinate_cache.rda' is empty, no data to return")
    return(NULL)
  }
  
  location <- vector(mode = "character", length = cache_len)
  status <- vector(mode = "double", length = cache_len)
  lon <- vector(mode = "double", length = cache_len)
  lat <- vector(mode = "double", length = cache_len)
  precise <- vector(mode = "integer", length = cache_len)
  confidence <- vector(mode = "integer", length = cache_len)
  comprehension <- vector(mode = "double", length = cache_len)
  level <- vector(mode = "character", length = cache_len)
  
  for (idx in seq_len(length(keys))) {
    curr_val <- bmap_env$coord_hash_map[[keys[idx]]]
    if (nzchar(curr_val[2])) {
      json_str <- jsonlite::fromJSON(curr_val[2])
    } else {
      json_str <- NULL
    }
    
    ## Unpack all of the variables.
    
    # Location string (input string).
    location[idx] <- curr_val[1]
    
    # Status
    if (!is.null(json_str$status)) {
      status[idx] <- json_str$status
    } else {
      status[idx] <- NA_real_
    }
    
    # Longitude
    if (!is.null(json_str$result$location$lng)) {
      lon[idx] <- json_str$result$location$lng
    } else {
      lon[idx] <- NA_real_
    }
    
    # Latitude
    if (!is.null(json_str$result$location$lat)) {
      lat[idx] <- json_str$result$location$lat
    } else {
      lat[idx] <- NA_real_
    }
    
    # Precise
    if (!is.null(json_str$result$precise)) {
      precise[idx] <- json_str$result$precise
    } else {
      precise[idx] <- NA_integer_
    }
    
    # Confidence
    if (!is.null(json_str$result$confidence)) {
      confidence[idx] <- json_str$result$confidence
    } else {
      confidence[idx] <- NA_integer_
    }
    
    # Comprehension
    if (!is.null(json_str$result$comprehension)) {
      comprehension[idx] <- json_str$result$comprehension
    } else {
      comprehension[idx] <- NA_integer_
    }
    
    # Level
    if (!is.null(json_str$result$level)) {
      level[idx] <- json_str$result$level
    } else {
      level[idx] <- NA_character_
    }
  }
  
  # initialize output df.
  out <- data.frame(
    location = location, 
    lon = lon, 
    lat = lat, 
    status = status, 
    precise = precise, 
    confidence = confidence, 
    comprehension = comprehension, 
    level = level, 
    stringsAsFactors = FALSE
  )
  
  out
}


#' Get Cached Address Data
#' 
#' Return all cached address data, as a tidy data frame.
#'
#' @return data frame
#' @export
#'
#' @examples \dontrun{
#' df <- bmap_get_cached_address_data()
#' }
bmap_get_cached_address_data <- function() {
  # Load the cache address data set.
  load_address_cache()
  cache_len <- length(bmap_env$addr_hash_map)
  keys <- names(bmap_env$addr_hash_map)
  
  # If length of the cached data set is zero, print message, then return NULL.
  if (cache_len == 0) {
    message("'address_cache.rda' is empty, no data to return")
    return(NULL)
  }
  
  input_lon <- vector(mode = "double", length = cache_len)
  input_lat <- vector(mode = "double", length = cache_len)
  return_lon <- vector(mode = "double", length = cache_len)
  return_lat <- vector(mode = "double", length = cache_len)
  status <- vector(mode = "double", length = cache_len)
  formatted_address <- vector(mode = "character", length = cache_len)
  business <- vector(mode = "character", length = cache_len)
  country <- vector(mode = "character", length = cache_len)
  country_code <- vector(mode = "double", length = cache_len)
  country_code_iso <- vector(mode = "character", length = cache_len)
  country_code_iso2 <- vector(mode = "character", length = cache_len)
  province <- vector(mode = "character", length = cache_len)
  city <- vector(mode = "character", length = cache_len)
  city_level <- vector(mode = "double", length = cache_len)
  district <- vector(mode = "character", length = cache_len)
  town <- vector(mode = "character", length = cache_len)
  ad_code <- vector(mode = "character", length = cache_len)
  street <- vector(mode = "character", length = cache_len)
  street_number <- vector(mode = "character", length = cache_len)
  direction <- vector(mode = "character", length = cache_len)
  distance <- vector(mode = "character", length = cache_len)
  semantic_desc <- vector(mode = "character", length = cache_len)
  city_code <- vector(mode = "double", length = cache_len)
  
  for (idx in seq_len(length(keys))) {
    curr_key <- keys[idx]
    res <- bmap_env$addr_hash_map[[curr_key]]
    if (nzchar(res)) {
      json_str <- jsonlite::fromJSON(res)
    } else {
      json_str <- NULL
    }
    
    ## Unpack all of the variables.
    
    # Input coordinates.
    input_coords <- get_coords_from_uri(curr_key)
    input_lon[idx] <- input_coords[["lon"]]
    input_lat[idx] <- input_coords[["lat"]]
    
    # Return Lon
    if (!is.null(json_str$result$location$lng)) {
      return_lon[idx] <- json_str$result$location$lng
    } else {
      return_lon[idx] <- NA_real_
    }
    
    # Return Lat
    if (!is.null(json_str$result$location$lat)) {
      return_lat[idx] <- json_str$result$location$lat
    } else {
      return_lat[idx] <- NA_real_
    }
    
    # Status
    if (!is.null(json_str$status)) {
      status[idx] <- json_str$status
    } else {
      status[idx] <- NA_real_
    }
    
    # Formatted Address
    if (!is.null(json_str$result$formatted_address)) {
      formatted_address[idx] <- json_str$result$formatted_address
    } else {
      formatted_address[idx] <- NA_character_
    }
    
    # Business
    if (!is.null(json_str$result$business)) {
      business[idx] <- json_str$result$business
    } else {
      business[idx] <- NA_character_
    }
    
    # Country
    if (!is.null(json_str$result$addressComponent$country)) {
      country[idx] <- json_str$result$addressComponent$country
    } else {
      country[idx] <- NA_character_
    }
    
    # Country Code
    if (!is.null(json_str$result$addressComponent$country_code)) {
      country_code[idx] <- json_str$result$addressComponent$country_code
    } else {
      country_code[idx] <- NA_real_
    }
    
    # Country Code ISO
    if (!is.null(json_str$result$addressComponent$country_code_iso)) {
      country_code_iso[idx] <- json_str$result$addressComponent$country_code_iso
    } else {
      country_code_iso[idx] <- NA_character_
    }
    
    # Country Code ISO2
    if (!is.null(json_str$result$addressComponent$country_code_iso2)) {
      country_code_iso2[idx] <- json_str$result$addressComponent$country_code_iso2
    } else {
      country_code_iso2[idx] <- NA_character_
    }
    
    # Province
    if (!is.null(json_str$result$addressComponent$province)) {
      province[idx] <- json_str$result$addressComponent$province
    } else {
      province[idx] <- NA_character_
    }
    
    # City
    if (!is.null(json_str$result$addressComponent$city)) {
      city[idx] <- json_str$result$addressComponent$city
    } else {
      city[idx] <- NA_character_
    }
    
    # City Level
    if (!is.null(json_str$result$addressComponent$city_level)) {
      city_level[idx] <- json_str$result$addressComponent$city_level
    } else {
      city_level[idx] <- NA_real_
    }
    
    # District
    if (!is.null(json_str$result$addressComponent$district)) {
      district[idx] <- json_str$result$addressComponent$district
    } else {
      district[idx] <- NA_real_
    }
    
    # Town
    if (!is.null(json_str$result$addressComponent$town)) {
      town[idx] <- json_str$result$addressComponent$town
    } else {
      town[idx] <- NA_character_
    }
    
    # adCode
    if (!is.null(json_str$result$addressComponent$adcode)) {
      ad_code[idx] <- json_str$result$addressComponent$adcode
    } else {
      ad_code[idx] <- NA_integer_
    }
    
    # Street
    if (!is.null(json_str$result$addressComponent$street)) {
      street[idx] <- json_str$result$addressComponent$street
    } else {
      street[idx] <- NA_character_
    }
    
    # Street Number
    if (!is.null(json_str$result$addressComponent$street_number)) {
      street_number[idx] <- json_str$result$addressComponent$street_number
    } else {
      street_number[idx] <- NA_character_
    }
    
    # Direction
    if (!is.null(json_str$result$addressComponent$direction)) {
      direction[idx] <- json_str$result$addressComponent$direction
    } else {
      direction[idx] <- NA_character_
    }
    
    # Distance
    if (!is.null(json_str$result$addressComponent$distance)) {
      distance[idx] <- json_str$result$addressComponent$distance
    } else {
      distance[idx] <- NA_character_
    }
    
    # Semantic Description
    if (!is.null(json_str$result$sematic_description)) {
      semantic_desc[idx] <- json_str$result$sematic_description
    } else {
      semantic_desc[idx] <- NA_character_
    }
    
    # City Code
    if (!is.null(json_str$result$cityCode)) {
      city_code[idx] <- json_str$result$cityCode
    } else {
      city_code[idx] <- NA_real_
    }
  }
  
  # initialize output df.
  out <- data.frame(
    input_lon = input_lon, 
    input_lat = input_lat, 
    return_lon = return_lon, 
    return_lat = return_lat, 
    status = status, 
    formatted_address = formatted_address, 
    business = business, 
    country = country, 
    country_code = country_code, 
    country_code_iso = country_code_iso, 
    country_code_iso2 = country_code_iso2, 
    province = province, 
    city = city, 
    city_level = city_level, 
    district = district, 
    town = town, 
    ad_code = ad_code, 
    street = street, 
    street_number = street_number, 
    direction = direction, 
    distance = distance, 
    semantic_desc = semantic_desc, 
    city_code = city_code, 
    stringsAsFactors = FALSE
  )
  
  out
}
