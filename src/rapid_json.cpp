#include <Rcpp.h>

// [[Rcpp::depends(rapidjsonr)]]
#include "rapidjson/document.h"

using namespace Rcpp;


// [[Rcpp::export]]
List from_json(const char* json) {
  rapidjson::Document doc;
  doc.Parse(json);
  
  double lng;
  double lat;
  
  // longitude
  if(doc["result"]["location"].HasMember("lng")) {
    lng = doc["result"]["location"]["lng"].GetDouble();
  } else {
    lng = NA_REAL;
  }
  
  // latitude
  if(doc["result"]["location"].HasMember("lat")) {
    lat = doc["result"]["location"]["lat"].GetDouble();
  } else {
    lat = NA_REAL;
  }
  
  List out = List::create(
    Named("lon") = lng,
    Named("lat") = lat
  );
  
  return out;
}


// [[Rcpp::export]]
List parse_api_json_coords(Environment& coord_hash_map,
                           CharacterVector& keys) {
  int cache_len = Rf_length(coord_hash_map);
  CharacterVector location(cache_len);
  NumericVector status(cache_len);
  NumericVector lng(cache_len);
  NumericVector lat(cache_len);
  IntegerVector precise(cache_len);
  IntegerVector confidence(cache_len);
  NumericVector comprehension(cache_len);
  CharacterVector level(cache_len);
  
  rapidjson::Document doc;
  rapidjson::Value val;
  CharacterVector curr_res;
  String curr_key;
  std::string curr_json;
  
  for(int i = 0; i < cache_len; ++i) {
    
    curr_key = keys[i];
    curr_res = coord_hash_map[curr_key];
    curr_json = as<std::string>(curr_res[1]);
    doc.Parse(curr_json.c_str());
    
    //location
    location[i] = curr_res[0];
    
    // status
    if(doc.HasMember("status")) {
      status[i] = doc["status"].GetDouble();
    } else {
      status[i] = NA_REAL;
    }
    
    // longitude
    if(doc["result"]["location"].HasMember("lng")) {
      lng[i] = doc["result"]["location"]["lng"].GetDouble();
    } else {
      lng[i] = NA_REAL;
    }
    
    // latitude
    if(doc["result"]["location"].HasMember("lat")) {
      lat[i] = doc["result"]["location"]["lat"].GetDouble();
    } else {
      lat[i] = NA_REAL;
    }
    
    // precise
    if(doc["result"].HasMember("precise")) {
      precise[i] = doc["result"]["precise"].GetInt();
    } else {
      precise[i] = NA_INTEGER;
    }
    
    // confidence
    if(doc["result"].HasMember("confidence")) {
      confidence[i] = doc["result"]["confidence"].GetInt();
    } else {
      confidence[i] = NA_INTEGER;
    }
    
    // comprehension
    if(doc["result"].HasMember("comprehension")) {
      comprehension[i] = doc["result"]["comprehension"].GetDouble();
    } else {
      comprehension[i] = NA_REAL;
    }
    
    // level
    if(doc["result"].HasMember("level")) {
      level[i] = doc["result"]["level"].GetString();
    } else {
      level[i] = NA_STRING;
    }
  }
  
  // Create List output that has the necessary attributes to make it a
  // data.frame object.
  List out = List::create(
    Named("location") = location,
    Named("status") = status,
    Named("lon") = lng,
    Named("lat") = lat,
    Named("presice") = precise,
    Named("confidence") = confidence,
    Named("comprehension") = comprehension,
    Named("level") = level
  );
  
  out.attr("class") = "data.frame";
  out.attr("row.names") = seq(1, cache_len);
  
  return out;
}
