
#' Get URI for a coordinates API query
#'
#' @noRd
get_coords_query_uri <- function(location) {
  paste0(
    "http://api.map.baidu.com/geocoding/v3/?address=", 
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
    "http://api.map.baidu.com/reverse_geocoding/v3/?ak=", 
     "%s", 
     "&location=", 
     lat, 
     ",", 
     lon, 
     "&output=json&coordtype=wgs84ll"
  )
}
