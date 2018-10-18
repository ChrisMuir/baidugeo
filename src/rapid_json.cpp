#include <Rcpp.h>

// [[Rcpp::depends(rapidjsonr)]]
#include "rapidjson/document.h"

using namespace Rcpp;


namespace global_vars {
  rapidjson::Document doc;
  //std::string curr_json;
  NumericVector status;
  NumericVector lng;
  NumericVector lat;
  IntegerVector precise;
  IntegerVector confidence;
  NumericVector comprehension;
  String level;
}


// Get coords data from the json of a single API request.
void from_json_coords(std::string json) {
  if(json != "") {
    global_vars::doc.Parse(json.c_str());
  } else {
    global_vars::doc.Parse("{\"empty\":\"json\"}");
  }
  
  if(global_vars::doc.HasParseError()) {
    //printf("json parse error: %d at %zu\n", static_cast<int>(doc.GetParseError()), doc.GetErrorOffset());
    Rcerr << "parse error for json string: "<< json << std::endl;
    stop("json parse error");
  }
  
  // status
  if(global_vars::doc.HasMember("status")) {
    global_vars::status[0] = global_vars::doc["status"].GetDouble();
  } else {
    global_vars::status[0] = NA_REAL;
  }
  
  // longitude
  if(global_vars::doc["result"]["location"].HasMember("lng")) {
    global_vars::lng[0] = global_vars::doc["result"]["location"]["lng"].GetDouble();
  } else {
    global_vars::lng[0] = NA_REAL;
  }
  
  // latitude
  if(global_vars::doc["result"]["location"].HasMember("lat")) {
    global_vars::lat[0] = global_vars::doc["result"]["location"]["lat"].GetDouble();
  } else {
    global_vars::lat[0] = NA_REAL;
  }
  
  // precise
  if(global_vars::doc["result"].HasMember("precise")) {
    global_vars::precise[0] = global_vars::doc["result"]["precise"].GetInt();
  } else {
    global_vars::precise[0] = NA_INTEGER;
  }
  
  // confidence
  if(global_vars::doc["result"].HasMember("confidence")) {
    global_vars::confidence[0] = global_vars::doc["result"]["confidence"].GetInt();
  } else {
    global_vars::confidence[0] = NA_INTEGER;
  }
  
  // comprehension
  if(global_vars::doc["result"].HasMember("comprehension")) {
    global_vars::comprehension[0] = global_vars::doc["result"]["comprehension"].GetDouble();
  } else {
    global_vars::comprehension[0] = NA_REAL;
  }
  
  // level
  if(global_vars::doc["result"].HasMember("level")) {
    global_vars::level = global_vars::doc["result"]["level"].GetString();
  } else {
    global_vars::level = NA_STRING;
  }
}


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
  
  // confidence
  std::string out;
  if(doc.HasMember("message")) {
    out = doc["message"].GetString();
  } else {
    out = "";
  }
  
  return out;
}


// [[Rcpp::export]]
List from_json_coords_vector (CharacterVector location, 
                              std::vector<std::string> json_vect) {
  int json_len = json_vect.size();
  NumericVector status(json_len);
  NumericVector lng(json_len);
  NumericVector lat(json_len);
  IntegerVector precise(json_len);
  IntegerVector confidence(json_len);
  NumericVector comprehension(json_len);
  CharacterVector level(json_len);
  
  for(int i = 0; i < json_len; ++i) {
    // Parse json, assign values from the parsed json.
    from_json_coords(json_vect[i]);
    status[i] = global_vars::status[0];
    lng[i] = global_vars::lng[0];
    lat[i] = global_vars::lat[0];
    precise[i] = global_vars::precise[0];
    confidence[i] = global_vars::confidence[0];
    comprehension[i] = global_vars::comprehension[0];
    level[i] = global_vars::level;
  }
  
  // Create List output that has the necessary attributes to make it a
  // data.frame object.
  List out = List::create(
    Named("location") = location,
    Named("lon") = lng,
    Named("lat") = lat,
    Named("status") = status,
    Named("presice") = precise,
    Named("confidence") = confidence,
    Named("comprehension") = comprehension,
    Named("level") = level
  );
  
  out.attr("class") = "data.frame";
  out.attr("row.names") = seq(1, json_len);
  
  return out;
}


// [[Rcpp::export]]
List get_coords_pkg_data(Environment& coord_hash_map,
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
  
  CharacterVector curr_res;
  String curr_key;

  for(int i = 0; i < cache_len; ++i) {
    
    curr_key = keys[i];
    curr_res = coord_hash_map[curr_key];

    //location
    location[i] = curr_res[0];
    
    // Parse json, assign values from the parsed json.
    from_json_coords(as<std::string>(curr_res[1]));
    status[i] = global_vars::status[0];
    lng[i] = global_vars::lng[0];
    lat[i] = global_vars::lat[0];
    precise[i] = global_vars::precise[0];
    confidence[i] = global_vars::confidence[0];
    comprehension[i] = global_vars::comprehension[0];
    level[i] = global_vars::level;
  }
  
  // Create List output that has the necessary attributes to make it a
  // data.frame object.
  List out = List::create(
    Named("location") = location,
    Named("lon") = lng,
    Named("lat") = lat,
    Named("status") = status,
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
List get_addrs_pkg_data(Environment& addr_hash_map,
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
  std::string curr_key;
  std::string curr_json;
  double curr_input_lon;
  double curr_input_lat;
  std::string temp_str;
  
  for(int i = 0; i < cache_len; ++i) {
    curr_key = as<std::string>(keys[i]);
    curr_json = as<std::string>(addr_hash_map[curr_key]);
    if(curr_json != "") {
      doc.Parse(curr_json.c_str());
    } else {
      doc.Parse("{\"empty\":\"json\"}");
    }
    
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
