context("bmap_set_key")

bmap_set_key("some_valid_key_str")
test_that("setting key is successful", {
  expect_equal(get("bmap_key", envir = baidugeo:::bmap_env), 
               "some_valid_key_str")
})


context("bmap_set_rate_limit")

bmap_set_daily_rate_limit(20000)
test_that("setting rate limit is successful", {
  expect_equal(get("bmap_daily_rate_limit", envir = baidugeo:::bmap_env), 
               20000L)
})


context("bmap_remaining_daily_queries")

test_that("bmap_remaining_daily_queries is successful", {
  expect_equal(bmap_remaining_daily_queries(), 20000L)
})


context("invalid_key_msg")

test_that("invalid key msg is correct", {
  expect_equal(
    invalid_key_msg('{\"message\":\"len of str is 3 or fewer chars\"}'), 
    paste0("API key is invalid. Current API key: some_valid_key_str\n    ", 
           "Response from the API:\n    ", 
           "len of str is 3 or fewer chars")
  )
})
