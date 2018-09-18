## Set up pkg environment to manage internal package data objects and rate 
## limiting.
bmap_env <- new.env()
assign("bmap_key", NULL, envir = bmap_env)
assign("bmap_daily_rate_limit", 5950L, envir = bmap_env)
assign("queries_left_today", 5950L, envir = bmap_env)
assign("next_limit_reset", NULL, envir = bmap_env)
assign("time_of_last_query", Sys.time(), envir = bmap_env)

# Initialize placeholders for package data within bmap_env.
assign("coord_hash_map", NULL, envir = bmap_env)
assign("addr_hash_map", NULL, envir = bmap_env)
