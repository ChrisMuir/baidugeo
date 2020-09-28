
#' Get URI for a coordinates API query
#'
#' @noRd
get_coords_query_uri <- function(location) {
  api_version <- get_api_version()
  if (api_version == "v2") {
    uri <-   paste0(
      "http://api.map.baidu.com/geocoder/v2/?address=",
      location, 
      "&output=json&ak=", 
      "%s"
    )
  } else {
    uri <-   paste0(
      "http://api.map.baidu.com/geocoding/v3/?address=", 
      location, 
      "&output=json&ak=", 
      "%s"
    )
  }
  uri
}


#' Get URI for an address API query
#'
#' @noRd
get_addr_query_uri <- function(lon, lat) {
  api_version <- get_api_version()
  if (api_version == "v2") {
    uri <- paste0(
      "http://api.map.baidu.com/geocoder/v2/?ak=", 
      "%s", 
      "&location=", 
      lat, 
      ",", 
      lon, 
      "&output=json&pois=0"
    )
  } else {
    uri <- paste0(
      "http://api.map.baidu.com/reverse_geocoding/v3/?ak=", 
      "%s", 
      "&location=", 
      lat, 
      ",", 
      lon, 
      "&output=json&coordtype=wgs84ll"
    )
  }
  uri
}

#' Get API version, either v2 or v3
#'
#' @noRd
get_api_version <- function() {
  get("api_key_version", envir = bmap_env)
}

#' Flip API version, between v2 and v3
#'
#' @noRd
flip_api_version <- function() {
  curr <- get_api_version()
  if (curr == "v2") {
    assign("api_key_version", "v3", envir = bmap_env)
  } else {
    assign("api_key_version", "v2", envir = bmap_env)
  }
}

#' Check to see if a json string response indicates the API key is not valid
#'
#' @noRd
invalid_key <- function(res) {
  grepl('\"message\":\"APP不存在，AK有误请检查再重试\"', res)
}

#' Check to see if a json string response indicates that an API key is not 
#' compatible with the current API version, as indicated by get_api_version()
#' 
#' @noRd
key_api_version_incompatible <- function(res) {
  grepl("APP 服务被禁用", res) && grepl('\"status\":240', res)
}

#' Error message indiating an API key is incompatible with both v2 and v3 
#' of the Baidu API
#'
#' @noRd
key_api_version_incompatible_msg <- function() {
  paste0(
    "Current API key is incompatible with both v2 and v3 of the Baidu API.\n",
    missing_key_msg()
    )
}
