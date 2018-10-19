
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
      if (is.null(bmap_env$coord_hash_map)) {
        assign("coord_hash_map", new.env(), envir = bmap_env)
      }
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
      if (is.null(bmap_env$addr_hash_map)) {
        assign("addr_hash_map", new.env(), envir = bmap_env)
      }
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
  
  get_coords_pkg_data(bmap_env$coord_hash_map, 
                      names(bmap_env$coord_hash_map))
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
  
  get_addrs_pkg_data(bmap_env$addr_hash_map, 
                     names(bmap_env$addr_hash_map))
}
