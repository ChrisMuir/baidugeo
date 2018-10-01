#include <Rcpp.h>

// [[Rcpp::depends(rapidjsonr)]]
#include "rapidjson/document.h"

using namespace Rcpp;


// Extract lon and lat as doubles from a uri string.
// Input uri's look like this:
// "http://api.map.baidu.com/geocoder/v2/?ak=%s&location=30.61,114.27&output=json&pois=0"
// [[Rcpp::export]]
void get_coords_from_uri(std::string& uri, double& curr_input_lon, 
                         double& curr_input_lat) {
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
  curr_input_lat = atof(uri.substr(0, pos).c_str());
  curr_input_lon = atof(uri.substr(pos + delim.size(), uri.find("&")).c_str());
}


// [[Rcpp::export]]
List from_json(const char* json) {
  rapidjson::Document doc;
  doc.Parse(json);
  
  if(doc.HasParseError()) {
    //printf("json parse error: %d at %zu\n", static_cast<int>(doc.GetParseError()), doc.GetErrorOffset());
    Rcerr << "parse error for json string: "<< json << std::endl;
    stop("json parse error");
  }
  
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
    
    if(doc.HasParseError()) {
      //printf("json parse error: %d at %zu\n", static_cast<int>(doc.GetParseError()), doc.GetErrorOffset());
      Rcerr << "parse error for json string: "<< curr_json << std::endl;
      stop("json parse error");
    }
    
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


// [[Rcpp::export]]
List parse_api_json_addrs(Environment& addr_hash_map,
                          CharacterVector& keys) {
  int cache_len = keys.size();
  NumericVector input_lon(cache_len);
  NumericVector input_lat(cache_len);
  NumericVector return_lon(cache_len);
  NumericVector return_lat(cache_len);
  NumericVector status(cache_len);
  CharacterVector formatted_address(cache_len);
  CharacterVector business(cache_len);
  CharacterVector country(cache_len);
  NumericVector country_code(cache_len);
  CharacterVector country_code_iso(cache_len);
  CharacterVector country_code_iso2(cache_len);
  CharacterVector province(cache_len);
  CharacterVector city(cache_len);
  NumericVector city_level(cache_len);
  CharacterVector district(cache_len);
  CharacterVector town(cache_len);
  IntegerVector ad_code(cache_len);
  CharacterVector street(cache_len);
  CharacterVector street_number(cache_len);
  CharacterVector direction(cache_len);
  CharacterVector distance(cache_len);
  CharacterVector sematic_desc(cache_len);
  CharacterVector city_code(cache_len);
  
  rapidjson::Document doc;
  rapidjson::Value val;
  std::string curr_key;
  std::string curr_json;
  double curr_input_lon;
  double curr_input_lat;
  std::string temp_str;
  
  for(int i = 0; i < cache_len; ++i) {
    curr_key = as<std::string>(keys[i]);
    curr_json = as<std::string>(addr_hash_map[curr_key]);
    doc.Parse(curr_json.c_str());
    
    if(doc.HasParseError()) {
      //printf("json parse error: %d at %zu\n", static_cast<int>(doc.GetParseError()), doc.GetErrorOffset());
      Rcerr << "parse error for json string: "<< curr_json << std::endl;
      stop("json parse error");
    }
    
    // Input lon and input lat.
    get_coords_from_uri(curr_key, curr_input_lon, curr_input_lat);
    input_lon[i] = curr_input_lon;
    input_lat[i] = curr_input_lat;
    
    // Return lon.
    if(doc["result"]["location"].HasMember("lng")) {
      return_lon[i] = doc["result"]["location"]["lng"].GetDouble();
    } else {
      return_lon[i] = NA_REAL;
    }
    
    // Return lat.
    if(doc["result"]["location"].HasMember("lat")) {
      return_lat[i] = doc["result"]["location"]["lat"].GetDouble();
    } else {
      return_lat[i] = NA_REAL;
    }
    
    // status
    if(doc.HasMember("status")) {
      status[i] = doc["status"].GetDouble();
    } else {
      status[i] = NA_REAL;
    }
    
    // formatted_address.
    if(doc["result"].HasMember("formatted_address")) {
      formatted_address[i] = doc["result"]["formatted_address"].GetString();
    } else {
      formatted_address[i] = NA_STRING;
    }
    
    // business.
    if(doc["result"].HasMember("business")) {
      business[i] = doc["result"]["business"].GetString();
    } else {
      business[i] = NA_STRING;
    }
    
    // country.
    if(doc["result"]["addressComponent"].HasMember("country")) {
      country[i] = doc["result"]["addressComponent"]["country"].GetString();
    } else {
      country[i] = NA_STRING;
    }
    
    // country_code.
    if(doc["result"]["addressComponent"].HasMember("country_code")) {
      country_code[i] = doc["result"]["addressComponent"]["country_code"].GetDouble();
    } else {
      country_code[i] = NA_REAL;
    }
    
    // country_code_iso.
    if(doc["result"]["addressComponent"].HasMember("country_code_iso")) {
      country_code_iso[i] = doc["result"]["addressComponent"]["country_code_iso"].GetString();
    } else {
      country_code_iso[i] = NA_STRING;
    }
    
    // country_code_iso2.
    if(doc["result"]["addressComponent"].HasMember("country_code_iso2")) {
      country_code_iso2[i] = doc["result"]["addressComponent"]["country_code_iso2"].GetString();
    } else {
      country_code_iso2[i] = NA_STRING;
    }
    
    // province.
    if(doc["result"]["addressComponent"].HasMember("province")) {
      province[i] = doc["result"]["addressComponent"]["province"].GetString();
    } else {
      province[i] = NA_STRING;
    }
    
    // city.
    if(doc["result"]["addressComponent"].HasMember("city")) {
      city[i] = doc["result"]["addressComponent"]["city"].GetString();
    } else {
      city[i] = NA_STRING;
    }
    
    // city_level.
    if(doc["result"]["addressComponent"].HasMember("city_level")) {
      city_level[i] = doc["result"]["addressComponent"]["city_level"].GetDouble();
    } else {
      city_level[i] = NA_REAL;
    }
    
    // district.
    if(doc["result"]["addressComponent"].HasMember("district")) {
      district[i] = doc["result"]["addressComponent"]["district"].GetString();
    } else {
      district[i] = NA_STRING;
    }
    
    // town.
    if(doc["result"]["addressComponent"].HasMember("town")) {
      town[i] = doc["result"]["addressComponent"]["town"].GetString();
    } else {
      town[i] = NA_STRING;
    }
    
    // adcode.
    if(doc["result"]["addressComponent"].HasMember("adcode")) {
      temp_str = doc["result"]["addressComponent"]["adcode"].GetString();
      ad_code[i] = stoi(temp_str);
    } else {
      ad_code[i] = NA_INTEGER;
    }
    
    // street.
    if(doc["result"]["addressComponent"].HasMember("street")) {
      street[i] = doc["result"]["addressComponent"]["street"].GetString();
    } else {
      street[i] = NA_STRING;
    }
    
    // street_number.
    if(doc["result"]["addressComponent"].HasMember("street_number")) {
      street_number[i] = doc["result"]["addressComponent"]["street_number"].GetString();
    } else {
      street_number[i] = NA_STRING;
    }
    
    // direction.
    if(doc["result"]["addressComponent"].HasMember("direction")) {
      direction[i] = doc["result"]["addressComponent"]["direction"].GetString();
    } else {
      direction[i] = NA_STRING;
    }
    
    // distance.
    if(doc["result"]["addressComponent"].HasMember("distance")) {
      distance[i] = doc["result"]["addressComponent"]["distance"].GetString();
    } else {
      distance[i] = NA_STRING;
    }
    
    // sematic_description.
    if(doc.HasMember("sematic_description")) {
      sematic_desc[i] = doc["sematic_description"].GetString();
    } else {
      sematic_desc[i] = NA_STRING;
    }
    
    // city_code.
    if(doc["result"].HasMember("cityCode")) {
      city_code[i] = doc["result"]["cityCode"].GetDouble();
    } else {
      city_code[i] = NA_REAL;
    }
  }
  
  // Create List output that has the necessary attributes to make it a
  // data.frame object.
  List out(23);
  out[0] = input_lon;
  out[1] = input_lat;
  out[2] = return_lon;
  out[3] = return_lat;
  out[4] = status;
  out[5] = formatted_address;
  out[6] = business;
  out[7] = country;
  out[8] = country_code;
  out[9] = country_code_iso;
  out[10] = country_code_iso2;
  out[11] = province;
  out[12] = city;
  out[13] = city_level;
  out[14] = district;
  out[15] = town;
  out[16] = ad_code;
  out[17] = street;
  out[18] = street_number;
  out[19] = direction;
  out[20] = distance;
  out[21] = sematic_desc;
  out[22] = city_code;
  
  CharacterVector names(23);
  names[0] = "input_lon";
  names[1] = "input_lat";
  names[2] = "return_lon";
  names[3] = "return_lat";
  names[4] = "status";
  names[5] = "formatted_address";
  names[6] = "business";
  names[7] = "country";
  names[8] = "country_code";
  names[9] = "country_code_iso";
  names[10] = "country_code_iso2";
  names[11] = "province";
  names[12] = "city";
  names[13] = "city_level";
  names[14] = "district";
  names[15] = "town";
  names[16] = "ad_code";
  names[17] = "street";
  names[18] = "street_number";
  names[19] = "direction";
  names[20] = "distance";
  names[21] = "sematic_desc";
  names[22] = "city_code";
  
  out.attr("names") = names;
  out.attr("class") = "data.frame";
  out.attr("row.names") = seq(1, cache_len);
  
  return out;
}
