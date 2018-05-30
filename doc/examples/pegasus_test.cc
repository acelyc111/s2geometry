// Copyright 2017 Google Inc. All Rights Reserved.
// Author: 405403881@xiaomi.com (acelyc111)
//
// simple usage of S2

#include <s2/s2latlng.h>
#include <s2/s2testing.h>

DEFINE_int32(num_index_points, 10000, "Number of points to index");
DEFINE_int32(num_queries, 10000, "Number of queries");
DEFINE_double(query_radius_km, 100, "Query radius in kilometers");

int main(int argc, char **argv) {
  S2LatLng latlng = S2LatLng::FromDegrees(40.039752, 116.332557);
  std::cout << latlng << std::endl;
  return  0;
}
