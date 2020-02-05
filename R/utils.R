
#' Get URI for a coordinates API query
#'
#' @noRd
get_coords_query_uri <- function(location) {
  paste0(
    "http://api.map.baidu.com/geocoder/v2/?address=", 
    location, 
    "&output=json&ak=", 
    "%s"
  )
}


#' Get URI for an address API query
#'
#' @noRd
get_addr_query_uri <- function(lon, lat) {
  paste0(
    "http://api.map.baidu.com/geocoder/v2/?ak=", 
     "%s", 
     "&location=", 
     lat, 
     ",", 
     lon, 
     "&output=json&pois=0"
  )
}
