% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/cache.R
\name{bmap_clear_cache}
\alias{bmap_clear_cache}
\title{Clear Cached Data Files}
\usage{
bmap_clear_cache(coordinate_cache = FALSE, address_cache = FALSE)
}
\arguments{
\item{coordinate_cache}{logical, if TRUE, all data currently in 
coordinate_cache.rda will be cleared. Default value is FALSE.}

\item{address_cache}{logical, if TRUE, all data currently in 
address_cache.rda will be cleared. Default value is FALSE.}
}
\value{
If both func args are set to FALSE, function will return NULL. 
 Otherwise, function does not return a value.
}
\description{
This function gives the user the ability to clear one or both of the cached 
data sets on file. Doing so for either of the datasets will completely 
erase all data for the dataset specified. Do NOT run this function if you 
don't want to loose any of the cached data currently on file.
}
\examples{
\dontrun{
# Clear both cached data sets.
bmap_clear_cache(coordinate_cache = TRUE, address_cache = TRUE)

# Clear only the address cache.
bmap_clear_cache(address_cache = TRUE)
}
}
