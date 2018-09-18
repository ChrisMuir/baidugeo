
#' Set Baidu Map API Key
#' 
#' Function for setting a Baidu Map API key. Key will be used in all API calls.
#'
#' @param key char string, valid Baidu Maps API key.
#'
#' @return Function does not return a value.
#' @export
#' 
#' @examples
#' bmap_set_key("some_valid_key_str")
#' 
bmap_set_key <- function(key) {
  stopifnot(is.character(key))
  assign("bmap_key", key, envir = bmap_env)
}


#' Set a daily rate limit for an API key.
#' 
#' This function will save the value passed to arg "num_limit" to the pkg 
#' environment value "bmap_daily_rate_limit", which is meant to be the number 
#' of daily queries allowed for the API key currently being used (default 
#' value for "bmap_daily_rate_limit" upon pkg load is 5950). Prior to using 
#' this function, a valid API key must be registered in the current R session, 
#' using function \code{\link{bmap_set_key}}.
#'
#' @param num_limit numeric value, number of daily API queries associated with 
#'  the valid key currently being used.
#'
#' @return Function does not return a value.
#' @export
#'
#' @examples \dontrun{
#' bmap_set_daily_rate_limit(20000)
#' }
#' 
bmap_set_daily_rate_limit <- function(num_limit) {
  if (num_limit != as.integer(num_limit) || num_limit < 1) {
    stop("arg 'num_limit' must be a whole number greater than zero")
  }
  num_limit <- as.integer(num_limit)
  
  # Check to make sure an API key has been registered.
  if (is.null(get("bmap_key", envir = bmap_env))) {
    stop(
      paste0("cannot set rate limit without first setting an API key.\n", 
             missing_key_msg()), 
      call. = FALSE
    )
  }
  
  # Assign the input rate limit to the pkg environment.
  assign("bmap_daily_rate_limit", num_limit, envir = bmap_env)
  assign("queries_left_today", num_limit, envir = bmap_env)
}


#' Get Rate Limit Details
#' 
#' Function that will return rate limit details related to the current 
#' registered API key. Values are returned as a list.
#' 
#' @details Function returns a list containing the following info
#' \itemize{
#' \item Current registered API key
#' \item Daily query limit of the current key
#' \item Number of daily queries remaining for the current 24 hour period
#' \item Date-time in which the daily query limit will reset.
#' }
#'
#' @return list of length four.
#' @export
#'
#' @examples
#' bmap_rate_limit_info()
#' 
bmap_rate_limit_info <- function() {
  # Initialize the output list.
  out <- list()
  
  out$current_key <- get("bmap_key", envir = bmap_env)
  out$daily_query_limit <- get("bmap_daily_rate_limit", envir = bmap_env)
  out$daily_queries_remaining <- bmap_remaining_daily_queries()
  reset <- get_limit_reset_time()
  if (is.null(reset)) {
    out$daily_limit_reset_time <- Sys.time() + 24*60*60
  } else {
    out$daily_limit_reset_time <- reset
  }
  
  out
}


#' Remaining Daily Queries
#' 
#' Get the number of daily queries left in the current 24 hour period, with 
#' the API key that is currently registered.
#'
#' @return integer, number of queries 
#' @export
#'
#' @examples
#' bmap_remaining_daily_queries()
#' 
bmap_remaining_daily_queries <- function() {
  get("queries_left_today", envir = bmap_env)
}


#' Get date-time in which the current 24 hour query limit will be reset
#' 
#' @noRd
get_limit_reset_time <- function() {
  get("next_limit_reset", envir = bmap_env)
}


#' Get timestamp of the last API query
#'
#' @noRd
timestamp_of_last_query <- function() {
  get("time_of_last_query", envir = bmap_env)
}


#' Query Limit Reset
#' 
#' Reset the values for "next_limit_reset" and "bmap_daily_rate_limit".
#'
#' @noRd
limit_reset <- function() {
  assign("next_limit_reset", Sys.time() + 24*60*60, envir = bmap_env)
  assign(
    "queries_left_today", 
    get("bmap_daily_rate_limit", envir = bmap_env), 
    envir = bmap_env
  )
}


#' Missing API key message
#'
#' @noRd
missing_key_msg <- function() {
  paste0("Apply for an application key here: ", 
         "http://lbsyun.baidu.com/apiconsole/key", 
         "\nThen register your key by running `bmap_set_key('valid_key_str')`")
}


#' Invalid API key message
#'
#' @noRd
invalid_key_msg <- function(api_res) {
  msg <- tryCatch(jsonlite::fromJSON(api_res), error = function(e) e)
  if (methods::is(api_res, "error")) {
    msg <- api_res
  } else {
    if ("message" %in% names(msg)) {
      msg <- msg$message
    } else {
      msg <- msg[grepl("APP.*AK.*", msg)]
      if (length(msg) < 1) {
        msg <- "unknown"
      } else {
        msg <- msg[[1]]
      }
    }
  }
  
  Encoding(msg) <- "UTF-8"
  
  sprintf(
    "API key is invalid. Current API key: %s
    Response from the API:
    %s",
    get("bmap_key", envir = bmap_env), msg
  )
}
