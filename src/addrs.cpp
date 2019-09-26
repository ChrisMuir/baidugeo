#include <Rcpp.h>
#include "baidugeo.h"
using namespace Rcpp;


// Get address data from the json of a single API request.
void from_json_addrs(std::string& json, std::string& key) {
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
  
  // Get MemberEnd() iterator.
  rapidjson::Value::ConstMemberIterator addr_comp_end = global_vars::doc["result"]["addressComponent"].MemberEnd();
  
  // Input lon and input lat (if "key" is not "NA").
  if(key != "NA") {
    get_coords_from_uri(key);
  }
  
  // Return lon.
  global_vars::iter = global_vars::doc["result"]["location"].FindMember("lng");
  if (global_vars::iter != global_vars::doc["result"]["location"].MemberEnd()) {
    global_vars::lng[0] = global_vars::iter->value.GetDouble();
  } else {
    global_vars::lng[0] = NA_REAL;
  }
  /*
  if(global_vars::doc["result"]["location"].HasMember("lng")) {
  global_vars::lng[0] = global_vars::doc["result"]["location"]["lng"].GetDouble();
  } else {
  global_vars::lng[0] = NA_REAL;
  }
  */
  
  // Return lat.
  global_vars::iter = global_vars::doc["result"]["location"].FindMember("lat");
  if (global_vars::iter != global_vars::doc["result"]["location"].MemberEnd()) {
    global_vars::lat[0] = global_vars::iter->value.GetDouble();
  } else {
    global_vars::lat[0] = NA_REAL;
  }
  /*
  if(global_vars::doc["result"]["location"].HasMember("lat")) {
  global_vars::lat[0] = global_vars::doc["result"]["location"]["lat"].GetDouble();
  } else {
  global_vars::lat[0] = NA_REAL;
  }
  */
  
  // status
  global_vars::iter = global_vars::doc.FindMember("status");
  if (global_vars::iter != global_vars::doc.MemberEnd()) {
    global_vars::status[0] = global_vars::iter->value.GetDouble();
  } else {
    global_vars::status[0] = NA_REAL;
  }
  /*
  if(global_vars::doc.HasMember("status")) {
  global_vars::status[0] = global_vars::doc["status"].GetDouble();
  } else {
  global_vars::status[0] = NA_REAL;
  }
  */
  
  // formatted_address.
  global_vars::iter = global_vars::doc["result"].FindMember("formatted_address");
  if (global_vars::iter != global_vars::doc["result"].MemberEnd()) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::formatted_address = addr_vars::temp_str;
    } else {
      addr_vars::formatted_address = NA_STRING;
    }
  } else {
    addr_vars::formatted_address = NA_STRING;
  }
  /*
  if(global_vars::doc["result"].HasMember("formatted_address")) {
  addr_vars::temp_str = global_vars::doc["result"]["formatted_address"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::formatted_address = addr_vars::temp_str;
  } else {
  addr_vars::formatted_address = NA_STRING;
  }
  } else {
  addr_vars::formatted_address = NA_STRING;
  }
  */
  
  // business.
  global_vars::iter = global_vars::doc["result"].FindMember("business");
  if (global_vars::iter != global_vars::doc["result"].MemberEnd()) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::business = addr_vars::temp_str;
    } else {
      addr_vars::business = NA_STRING;
    }
  } else {
    addr_vars::business = NA_STRING;
  }
  /*
  if(global_vars::doc["result"].HasMember("business")) {
  addr_vars::temp_str = global_vars::doc["result"]["business"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::business = addr_vars::temp_str;
  } else {
  addr_vars::business = NA_STRING;
  }
  } else {
  addr_vars::business = NA_STRING;
  }
  */
  
  // country.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("country");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::country = addr_vars::temp_str;
    } else {
      addr_vars::country = NA_STRING;
    }
  } else {
    addr_vars::country = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("country")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["country"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::country = addr_vars::temp_str;
  } else {
  addr_vars::country = NA_STRING;
  }
  } else {
  addr_vars::country = NA_STRING;
  }
  */
  
  // country_code.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("country_code");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::country_code[0] = global_vars::iter->value.GetDouble();
  } else {
    addr_vars::country_code[0] = NA_REAL;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("country_code")) {
  addr_vars::country_code[0] = global_vars::doc["result"]["addressComponent"]["country_code"].GetDouble();
  } else {
  addr_vars::country_code[0] = NA_REAL;
  }
  */
  
  // country_code_iso.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("country_code_iso");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::country_code_iso = addr_vars::temp_str;
    } else {
      addr_vars::country_code_iso = NA_STRING;
    }
  } else {
    addr_vars::country_code_iso = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("country_code_iso")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["country_code_iso"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::country_code_iso = addr_vars::temp_str;
  } else {
  addr_vars::country_code_iso = NA_STRING;
  }
  } else {
  addr_vars::country_code_iso = NA_STRING;
  }
  */
  
  // country_code_iso2.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("country_code_iso2");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::country_code_iso2 = addr_vars::temp_str;
    } else {
      addr_vars::country_code_iso2 = NA_STRING;
    }
  } else {
    addr_vars::country_code_iso2 = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("country_code_iso2")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["country_code_iso2"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::country_code_iso2 = addr_vars::temp_str;
  } else {
  addr_vars::country_code_iso2 = NA_STRING;
  }
  } else {
  addr_vars::country_code_iso2 = NA_STRING;
  }
  */
  
  // province.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("province");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::province = addr_vars::temp_str;
    } else {
      addr_vars::province = NA_STRING;
    }
  } else {
    addr_vars::province = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("province")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["province"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::province = addr_vars::temp_str;
  } else {
  addr_vars::province = NA_STRING;
  }
  } else {
  addr_vars::province = NA_STRING;
  }
  */
  
  // city.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("city");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::city = addr_vars::temp_str;
    } else {
      addr_vars::city = NA_STRING;
    }
  } else {
    addr_vars::city = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("city")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["city"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::city = addr_vars::temp_str;
  } else {
  addr_vars::city = NA_STRING;
  }
  } else {
  addr_vars::city = NA_STRING;
  }
  */
  
  // city_level.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("city_level");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::city_level[0] = global_vars::iter->value.GetDouble();
  } else {
    addr_vars::city_level[0] = NA_REAL;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("city_level")) {
  addr_vars::city_level[0] = global_vars::doc["result"]["addressComponent"]["city_level"].GetDouble();
  } else {
  addr_vars::city_level[0] = NA_REAL;
  }
  */
  
  // district.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("district");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::district = addr_vars::temp_str;
    } else {
      addr_vars::district = NA_STRING;
    }
  } else {
    addr_vars::district = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("district")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["district"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::district = addr_vars::temp_str;
  } else {
  addr_vars::district = NA_STRING;
  }
  } else {
  addr_vars::district = NA_STRING;
  }
  */
  
  // town.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("town");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::town = addr_vars::temp_str;
    } else {
      addr_vars::town = NA_STRING;
    }
  } else {
    addr_vars::town = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("town")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["town"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::town = addr_vars::temp_str;
  } else {
  addr_vars::town = NA_STRING;
  }
  } else {
  addr_vars::town = NA_STRING;
  }
  */
  
  // adcode.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("adcode");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::ad_code[0] = stoi(addr_vars::temp_str);
    } else {
      addr_vars::ad_code[0] = NA_INTEGER;
    }
  } else {
    addr_vars::ad_code[0] = NA_INTEGER;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("adcode")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["adcode"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::ad_code[0] = stoi(addr_vars::temp_str);
  } else {
  addr_vars::ad_code[0] = NA_INTEGER;
  }
  } else {
  addr_vars::ad_code[0] = NA_INTEGER;
  }
  */
  
  // street.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("street");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::street = addr_vars::temp_str;
    } else {
      addr_vars::street = NA_STRING;
    }
  } else {
    addr_vars::street = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("street")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["street"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::street = addr_vars::temp_str;
  } else {
  addr_vars::street = NA_STRING;
  }
  } else {
  addr_vars::street = NA_STRING;
  }
  */
  
  // street_number.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("street_number");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::street_number = addr_vars::temp_str;
    } else {
      addr_vars::street_number = NA_STRING;
    }
  } else {
    addr_vars::street_number = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("street_number")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["street_number"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::street_number = addr_vars::temp_str;
  } else {
  addr_vars::street_number = NA_STRING;
  }
  } else {
  addr_vars::street_number = NA_STRING;
  }
  */
  
  // direction.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("direction");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::direction = addr_vars::temp_str;
    } else {
      addr_vars::direction = NA_STRING;
    }
  } else {
    addr_vars::direction = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("direction")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["direction"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::direction = addr_vars::temp_str;
  } else {
  addr_vars::direction = NA_STRING;
  }
  } else {
  addr_vars::direction = NA_STRING;
  }
  */
  
  // distance.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("distance");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::distance[0] = stoi(addr_vars::temp_str);
    } else {
      addr_vars::distance[0] = NA_INTEGER;
    }
  } else {
    addr_vars::distance[0] = NA_INTEGER;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("distance")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["distance"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::distance = stoi(addr_vars::temp_str);
  } else {
  addr_vars::distance = NA_INTEGER;
  }
  } else {
  addr_vars::distance = NA_INTEGER;
  }
  */
  
  // sematic_description.
  global_vars::iter = global_vars::doc["result"]["addressComponent"].FindMember("sematic_description");
  if (global_vars::iter != addr_comp_end) {
    addr_vars::temp_str = global_vars::iter->value.GetString();
    if(addr_vars::temp_str != "") {
      addr_vars::sematic_desc = addr_vars::temp_str;
    } else {
      addr_vars::sematic_desc = NA_STRING;
    }
  } else {
    addr_vars::sematic_desc = NA_STRING;
  }
  /*
  if(global_vars::doc["result"]["addressComponent"].HasMember("sematic_description")) {
  addr_vars::temp_str = global_vars::doc["result"]["addressComponent"]["sematic_description"].GetString();
  if(addr_vars::temp_str != "") {
  addr_vars::sematic_desc = addr_vars::temp_str;
  } else {
  addr_vars::sematic_desc = NA_STRING;
  }
  } else {
  addr_vars::sematic_desc = NA_STRING;
  }
  */
  
  // city_code.
  global_vars::iter = global_vars::doc["result"].FindMember("cityCode");
  if (global_vars::iter != global_vars::doc["result"].MemberEnd()) {
    addr_vars::city_code[0] = global_vars::iter->value.GetDouble();
  } else {
    addr_vars::city_code[0] = NA_REAL;
  }
  /*
  if(global_vars::doc["result"].HasMember("cityCode")) {
  addr_vars::city_code[0] = global_vars::doc["result"]["cityCode"].GetDouble();
  } else {
  addr_vars::city_code[0] = NA_REAL;
  }
  */
}


// [[Rcpp::export]]
List from_json_addrs_vector(NumericVector lng, NumericVector lat, 
                            std::vector<std::string> json_vect) {
  int json_len = json_vect.size();
  NumericVector input_lng(json_len);
  NumericVector input_lat(json_len);
  NumericVector return_lng(json_len);
  NumericVector return_lat(json_len);
  NumericVector status(json_len);
  CharacterVector formatted_address(json_len);
  CharacterVector business(json_len);
  CharacterVector country(json_len);
  NumericVector country_code(json_len);
  CharacterVector country_code_iso(json_len);
  CharacterVector country_code_iso2(json_len);
  CharacterVector province(json_len);
  CharacterVector city(json_len);
  NumericVector city_level(json_len);
  CharacterVector district(json_len);
  CharacterVector town(json_len);
  IntegerVector ad_code(json_len);
  CharacterVector street(json_len);
  CharacterVector street_number(json_len);
  CharacterVector direction(json_len);
  IntegerVector distance(json_len);
  CharacterVector sematic_desc(json_len);
  NumericVector city_code(json_len);
  
  rapidjson::Document doc;
  std::string na_str = "NA";
  
  for(int i = 0; i < json_len; ++i) {
    from_json_addrs(json_vect[i], na_str);
    
    input_lng[i] = lng[i];
    input_lat[i] = lat[i];
    return_lng[i] = global_vars::lng[0];
    return_lat[i] = global_vars::lat[0];
    status[i] = global_vars::status[0];
    formatted_address[i] = addr_vars::formatted_address;
    business[i] = addr_vars::business;
    country[i] = addr_vars::country;
    country_code[i] = addr_vars::country_code[0];
    country_code_iso[i] = addr_vars::country_code_iso;
    country_code_iso2[i] = addr_vars::country_code_iso2;
    province[i] = addr_vars::province;
    city[i] = addr_vars::city;
    city_level[i] = addr_vars::city_level[0];
    district[i] = addr_vars::district;
    town[i] = addr_vars::town;
    ad_code[i] = addr_vars::ad_code[0];
    street[i] = addr_vars::street;
    street_number[i] = addr_vars::street_number;
    direction[i] = addr_vars::direction;
    distance[i] = addr_vars::distance[0];
    sematic_desc[i] = addr_vars::sematic_desc;
    city_code[i] = addr_vars::city_code[0];
  }
  
  // Create List output that has the necessary attributes to make it a
  // data.frame object.
  List out(23);
  out[0] = input_lng;
  out[1] = input_lat;
  out[2] = return_lng;
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
  out.attr("row.names") = seq(1, json_len);
  
  return out;
}


// [[Rcpp::export]]
List get_addrs_pkg_data(Environment& addr_hash_map,
                        CharacterVector& keys) {
  int cache_len = keys.size();
  NumericVector input_lng(cache_len);
  NumericVector input_lat(cache_len);
  NumericVector return_lng(cache_len);
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
  IntegerVector distance(cache_len);
  CharacterVector sematic_desc(cache_len);
  NumericVector city_code(cache_len);
  
  std::string curr_key;
  std::string curr_json;
  
  for(int i = 0; i < cache_len; ++i) {
    curr_key = as<std::string>(keys[i]);
    curr_json = as<std::string>(addr_hash_map[curr_key]);
    
    from_json_addrs(curr_json, curr_key);
    
    input_lng[i] = addr_vars::input_lng;
    input_lat[i] = addr_vars::input_lat;
    return_lng[i] = global_vars::lng[0];
    return_lat[i] = global_vars::lat[0];
    status[i] = global_vars::status[0];
    formatted_address[i] = addr_vars::formatted_address;
    business[i] = addr_vars::business;
    country[i] = addr_vars::country;
    country_code[i] = addr_vars::country_code[0];
    country_code_iso[i] = addr_vars::country_code_iso;
    country_code_iso2[i] = addr_vars::country_code_iso2;
    province[i] = addr_vars::province;
    city[i] = addr_vars::city;
    city_level[i] = addr_vars::city_level[0];
    district[i] = addr_vars::district;
    town[i] = addr_vars::town;
    ad_code[i] = addr_vars::ad_code[0];
    street[i] = addr_vars::street;
    street_number[i] = addr_vars::street_number;
    direction[i] = addr_vars::direction;
    distance[i] = addr_vars::distance[0];
    sematic_desc[i] = addr_vars::sematic_desc;
    city_code[i] = addr_vars::city_code[0];
  }
  
  // Create List output that has the necessary attributes to make it a
  // data.frame object.
  List out(23);
  out[0] = input_lng;
  out[1] = input_lat;
  out[2] = return_lng;
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
