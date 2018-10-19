#include <Rcpp.h>
#include "baidugeo.h"
using namespace Rcpp;


// [[Rcpp::export]]
bool is_json_parsable(const char * json) {
  rapidjson::Document doc;
  doc.Parse(json);
  
  if(doc.HasParseError()) {
    return false;
  }
  
  return true;
}


// [[Rcpp::export]]
std::string get_message_value(const char * json) {
  rapidjson::Document doc;
  doc.Parse(json);
  
  std::string out;
  if(doc.HasMember("message")) {
    out = doc["message"].GetString();
  } else {
    out = "";
  }
  
  return out;
}


// Extract lon and lat as doubles from a uri string.
// Input uri's look like this:
// "http://api.map.baidu.com/geocoder/v2/?ak=%s&location=30.61,114.27&output=json&pois=0"
void get_coords_from_uri(std::string& uri) {
  size_t pos;
  std::string delim;
  
  // split by delim "location=", keep only the second half.
  // Result will look like this:
  // "30.61,114.27&output=json&pois=0"
  delim = "location=";
  pos = uri.find(delim);
  uri = uri.substr(pos + delim.size(), uri.size() - 1);
  
  // Split by delims "," and "&" to extract the lat and lon.
  delim = ",";
  pos = uri.find(delim);
  addr_vars::input_lat = atof(uri.substr(0, pos).c_str());
  addr_vars::input_lng = atof(uri.substr(pos + delim.size(), uri.find("&")).c_str());
}
