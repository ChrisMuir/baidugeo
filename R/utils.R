
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


#' Get coords from an API uri
#'
#' @noRd
get_coords_from_uri <- function(uri) {
  uri <- strsplit(uri, "location=", fixed = TRUE)[[1]][2]
  uri <- strsplit(uri, ",", fixed = TRUE)
  lat <- as.double(uri[[1]][1])
  lon <- as.double(strsplit(uri[[1]][2], "&", fixed = TRUE)[[1]][1])
  c("lat" = lat, "lon" = lon)
}
