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
