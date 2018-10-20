
<!-- README.md is generated from README.Rmd. Please edit that file -->
baidugeo
========

[![Travis-CI Build Status](https://travis-ci.org/ChrisMuir/baidugeo.svg?branch=master)](https://travis-ci.org/ChrisMuir/baidugeo)

R interface for the Baidu Maps API geocoding services. API docs can be found [here](http://lbsyun.baidu.com/index.php?title=jspopular), [here](http://developer.baidu.com/map/skins/MySkin/resources/iframs/webapi-geocoding.html), and [here](http://lbsyun.baidu.com/index.php?title=car/api/geocoding). Functions are provided for both forward and reverse geocoding. The Baidu Maps API is useful for geolocation of Chinese strings (addresses, business names, locations, etc).

This project initially started as a fork of the [baidumap](https://github.com/badbye/baidumap) package by [Yalei Du](https://github.com/badbye). This package is meant to be a stripped down, simplified version of that package, useful for when all you need is geocoding from Baidu, and with a couple additional features built in. Major differences include:

-   All API return data is cached as internal package data. The cache is persistent across R sessions.
-   API rate limiting is handled automatically.
-   Removes multicore processing, so no parallel API calls.
-   Fewer dependencies.
-   Uses the [rapidjson](https://github.com/Tencent/rapidjson) C++ header files (via the R packge [rapidjsonr](https://github.com/SymbolixAU/rapidjsonr)) for fast parsing of JSON API return data.

For geolocation of Chinese terms using basic string parsing (no API required), check out my package [geolocChina](https://github.com/ChrisMuir/geolocChina).

Install from this repo
----------------------

``` r
#install.packages("devtools")
devtools::install_github("ChrisMuir/baidugeo")
```

Usage and API Credentialing
---------------------------

A valid key is required to make queries with this package. Apply for an application key from [lbsyun.baidu.com](http://lbsyun.baidu.com/apiconsole/key). Then register your key using function `bmap_set_key()`.

``` r
library(baidugeo)
bmap_set_key("some_valid_key_str")
```

The Baidu Maps API features daily rate limits. By default, any key registered using `bmap_set_key()` will be assigned a daily rate limit of 5950. This can be adjusted using function `bmap_set_daily_rate_limit()`

``` r
bmap_set_daily_rate_limit(20000)
```

The function `bmap_rate_limit_info()` can be used to get info related to the currently registered key.

``` r
bmap_rate_limit_info()
#> $current_key
#> [1] "some_valid_key_str"
#> 
#> $daily_query_limit
#> [1] 20000
#> 
#> $daily_queries_remaining
#> [1] 20000
#> 
#> $daily_limit_reset_time
#> [1] "2018-10-20 20:49:27 EDT"
```

Geocoding
---------

Use function `bmap_get_coords()` to get lat and lon coordinates given a vector of strings.

``` r
locs <- c(
  "中百超市有限公司长堤街二分店", 
  "浙江省杭州市余杭区径山镇小古城村", 
  "成都高梁红餐饮管理有限公司"
)
```

`bmap_get_coords()` returns a data frame by default.

``` r
coords_df <- bmap_get_coords(locs)
knitr::kable(coords_df)
```

| location                         |       lon|       lat|  status|  precise|  confidence|  comprehension| level   |
|:---------------------------------|---------:|---------:|-------:|--------:|-----------:|--------------:|:--------|
| 中百超市有限公司长堤街二分店     |  114.2729|  30.61617|       0|        1|          80|             95| UNKNOWN |
| 浙江省杭州市余杭区径山镇小古城村 |  119.8783|  30.39625|       0|        0|          30|            100| 乡镇    |
| 成都高梁红餐饮管理有限公司       |  104.0679|  30.67994|       0|        0|          12|             29| 城市    |

Use arg `type` to return a vector of json strings.

``` r
bmap_get_coords(locs, type = "json")
```

    #> [1] "{\"status\":0,\"result\":{\"location\":{\"lng\":114.27287244473057,\"lat\":30.616167082550779},\"precise\":1,\"confidence\":80,\"comprehension\":95,\"level\":\"UNKNOWN\"}}"
    #> [2] "{\"status\":0,\"result\":{\"location\":{\"lng\":119.87833669326516,\"lat\":30.39624844375698},\"precise\":0,\"confidence\":30,\"comprehension\":100,\"level\":\"乡镇\"}}"   
    #> [3] "{\"status\":0,\"result\":{\"location\":{\"lng\":104.06792346330406,\"lat\":30.679942845419565},\"precise\":0,\"confidence\":12,\"comprehension\":29,\"level\":\"城市\"}}"

Reverse Geocoding
-----------------

Use function `bmap_get_location()` to get full address details given vectors of lat/lon coordinate pairs.

`bmap_get_location()` returns a data frame by default.

``` r
addrs_df <- bmap_get_location(coords_df$lat, coords_df$lon)
knitr::kable(addrs_df)
```

|  input\_lon|  input\_lat|  return\_lon|  return\_lat|  status| formatted\_address             | business               | country |  country\_code| country\_code\_iso | country\_code\_iso2 | province | city   |  city\_level| district | town |  ad\_code| street   | street\_number | direction |  distance|  city\_code|
|-----------:|-----------:|------------:|------------:|-------:|:-------------------------------|:-----------------------|:--------|--------------:|:-------------------|:--------------------|:---------|:-------|------------:|:---------|:-----|---------:|:---------|:---------------|:----------|---------:|-----------:|
|    114.2729|    30.61617|     114.2729|     30.61617|       0| 湖北省武汉市江汉区新华路630号  | 汉口火车站,常青路,北湖 | 中国    |              0| CHN                | CN                  | 湖北省   | 武汉市 |            2| 江汉区   | NA   |    420103| 新华路   | 630号          | 附近      |         9|         218|
|    119.8783|    30.39625|     119.8783|     30.39625|       0| 浙江省杭州市余杭区潘金线       | NA                     | 中国    |              0| CHN                | CN                  | 浙江省   | 杭州市 |            2| 余杭区   | NA   |    330110| 潘金线   | NA             | NA        |        NA|         179|
|    104.0679|    30.67994|     104.0679|     30.67994|       0| 四川省成都市青羊区王家塘街84号 | 骡马市,新华西路,八宝街 | 中国    |              0| CHN                | CN                  | 四川省   | 成都市 |            2| 青羊区   | NA   |    510105| 王家塘街 | 84号           | 附近      |         6|          75|

Use arg `type` to return a vector of json strings.

``` r
bmap_get_location(coords_df$lat, coords_df$lon, type = "json")
```

    #> [1] "{\"status\":0,\"result\":{\"location\":{\"lng\":114.27287244473092,\"lat\":30.61616696729939},\"formatted_address\":\"湖北省武汉市江汉区新华路630号\",\"business\":\"汉口火车站,常青路,北湖\",\"addressComponent\":{\"country\":\"中国\",\"country_code\":0,\"country_code_iso\":\"CHN\",\"country_code_iso2\":\"CN\",\"province\":\"湖北省\",\"city\":\"武汉市\",\"city_level\":2,\"district\":\"江汉区\",\"town\":\"\",\"adcode\":\"420103\",\"street\":\"新华路\",\"street_number\":\"630号\",\"direction\":\"附近\",\"distance\":\"9\"},\"pois\":[],\"roads\":[],\"poiRegions\":[],\"sematic_description\":\"锦江之星酒店(武汉菱角湖万达店)南53米\",\"cityCode\":218}}"                                                                                                                             
    #> [2] "{\"status\":0,\"result\":{\"location\":{\"lng\":119.87833669326493,\"lat\":30.39624841472855},\"formatted_address\":\"浙江省杭州市余杭区潘金线\",\"business\":\"\",\"addressComponent\":{\"country\":\"中国\",\"country_code\":0,\"country_code_iso\":\"CHN\",\"country_code_iso2\":\"CN\",\"province\":\"浙江省\",\"city\":\"杭州市\",\"city_level\":2,\"district\":\"余杭区\",\"town\":\"\",\"adcode\":\"330110\",\"street\":\"潘金线\",\"street_number\":\"\",\"direction\":\"\",\"distance\":\"\"},\"pois\":[],\"roads\":[],\"poiRegions\":[],\"sematic_description\":\"阳坞山西北444米\",\"cityCode\":179}}"                                                                                                                                                                                       
    #> [3] "{\"status\":0,\"result\":{\"location\":{\"lng\":104.06792346330394,\"lat\":30.67994271533221},\"formatted_address\":\"四川省成都市青羊区王家塘街84号\",\"business\":\"骡马市,新华西路,八宝街\",\"addressComponent\":{\"country\":\"中国\",\"country_code\":0,\"country_code_iso\":\"CHN\",\"country_code_iso2\":\"CN\",\"province\":\"四川省\",\"city\":\"成都市\",\"city_level\":2,\"district\":\"青羊区\",\"town\":\"\",\"adcode\":\"510105\",\"street\":\"王家塘街\",\"street_number\":\"84号\",\"direction\":\"附近\",\"distance\":\"6\"},\"pois\":[],\"roads\":[],\"poiRegions\":[{\"direction_desc\":\"内\",\"name\":\"青羊区政府\",\"tag\":\"政府机构;各级政府\",\"uid\":\"96b672aa58335874cf04ef80\"}],\"sematic_description\":\"青羊区政府内,成都华氏陶瓷艺术博物馆附近1米\",\"cityCode\":75}}"

Package Data
------------

Functions `bmap_get_coords()` and `bmap_get_location()` both cache API return data. The functions will first look for the return data in the cached package datasets, if it's not found there they will execute an API request.

Use function `bmap_clear_cache()` to clear either of the package cache data sets (or both at once).

``` r
bmap_clear_cache(coordinate_cache = TRUE, address_cache = TRUE)
```

Use function `bmap_get_cached_coord_data()` to load all of the cached lat/lon return data as a tidy data frame.

``` r
df <- bmap_get_cached_coord_data()
```

Use function `bmap_get_cached_address_data()` to load the cached address return data as a tidy data frame.

``` r
df <- bmap_get_cached_address_data()
```
