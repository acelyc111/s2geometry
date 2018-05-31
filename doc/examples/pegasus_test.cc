// Copyright 2017 Google Inc. All Rights Reserved.
// Author: 405403881@xiaomi.com (acelyc111)
//
// simple usage of S2

#include <iomanip>
#include <s2/s2latlng.h>
#include <s2/s2cell.h>
#include <s2/s2earth.h>
#include <s2/s2latlng_rect.h>
#include <s2/s2cap.h>
#include <s2/s2testing.h>
#include <s2/s2region_coverer.h>
#include <s2/s2polygon.h>

DEFINE_int32(num_index_points, 10000, "Number of points to index");
DEFINE_int32(num_queries, 10000, "Number of queries");
DEFINE_double(query_radius_km, 100, "Query radius in kilometers");

int main(int argc, char **argv) {
  // latlng
  S2LatLng latlng = S2LatLng::FromDegrees(40.039752, 116.332557);
  std::cout << "latlng: " << latlng << std::endl;

  // leaf cell
  S2Cell cell(latlng);
  std::cout << "cell: ["
            << "id: " << cell.id()
            << ", face: " << cell.face()
            << ", level: " << cell.level()
            << "]" << std::endl;

  // leaf cell id
  S2CellId cell_id = cell.id();
  std::cout << "cell_id: " << cell_id.ToString()
            << ", id: " << cell_id.id() << std::endl;

  // convert to a parent level cell
  S2CellId parent_cell_id = cell_id.parent(24);     // level 24 error less than 1 meter
  std::cout << std::left
            << "parent cell id"
            << ", level: " << parent_cell_id.level()
            << ", cell_id: " << parent_cell_id.ToString()
            << ", id: " << parent_cell_id.id();
  S2Cell parent_cell(parent_cell_id);std::cout << std::left
            << ", ApproxArea: " << S2Earth::SteradiansToSquareMeters(parent_cell.ApproxArea()) << std::endl;

  // traverse from leaf to face(top level)
  while (!cell_id.is_face()) {
      cell_id = cell_id.parent();
      std::cout << std::left
                << "level: " << std::setw(2) << cell_id.level()
                << ", cell_id: " << std::setw(31) << cell_id.ToString()
                << ", id: " << cell_id.id() << std::endl;
  }

  // region cover
  S2RegionCoverer rc;
//  rc.mutable_options()->set_min_level(7);
//  rc.mutable_options()->set_max_level(24);
  rc.mutable_options()->set_min_level(14);
  rc.mutable_options()->set_max_level(14);
  rc.mutable_options()->set_max_cells(30);

  S2CellUnion cell_union;
/*  // rect
  // (low-left, up-right)
  S2LatLngRect rect(S2LatLng::FromDegrees(40.038725, 116.331329),
                    S2LatLng::FromDegrees(40.042097, 116.333684));
  cell_union = rc.GetCovering(rect);
  std::cout << "rect: " << rect << std::endl;
  for (auto &ci : cell_union) {
      std::cout << ci.level() << ": " << ci.id() << std::endl;
  }
  std::cout << std::endl;*/

  // cap
  S2Cap cap(S2LatLng::FromDegrees(40.039752, 116.332557).ToPoint(),
            S2Earth::ToAngle(util::units::Meters(50000.0)));

  int min_level = 10;
  int max_level = 14;
  rc.mutable_options()->set_min_level(min_level);
  rc.mutable_options()->set_max_level(max_level);
  S2CellUnion cell_union1 = rc.GetCovering(cap);    // GetInteriorCovering
  std::cout << "cap: " << cap << std::endl;
  std::cout << "cell_union11: " << cell_union1.size() << ", normal: " << cell_union1.IsNormalized() << std::endl;
  std::vector<S2CellId> cell_ids;
  for (auto &ci : cell_union1) {
      if (ci.level() != min_level &&
          ci.level() != max_level) {
          for(S2CellId c = ci.child_begin(max_level);
                  c != ci.child_end(max_level);
                  c = c.next()) {
              cell_ids.push_back(c);
          }
      } else {
          std::cout << ci.level() << ": " << ci.id() << std::endl;
          cell_ids.push_back(ci);
      }
  }

  std::cout << "cell_ids11.size: " << cell_ids.size() << std::endl;
//  for (auto ci : cell_ids) {
//      if (ci.level() != max_level) {
//          std::cout << ci.level() << ": " << ci.id() << std::endl;
//      }
//  }

  std::cout << "===================" << std::endl;
  rc.mutable_options()->set_fixed_level(10);
  S2CellUnion cell_union11 = rc.GetCovering(cap);
  std::cout << "size: " << cell_union11.size() << std::endl;
  for (auto &ci : cell_union11) {
      if (cap.Contains(S2Cell(ci))) {
          std::cout << "full: " << ci.level() << ": " << ci << std::endl;
      } else {
        std::vector<S2CellId> cell_ids11;
        for (S2CellId c = ci.child_begin(18);
             c != ci.child_end(18);
             c = c.next()) {
          if (cap.MayIntersect(S2Cell(c))) {
            cell_ids11.push_back(c);
          }
        }

//        for (auto &ci2 : cell_ids11) {
//          std::cout << ci2.level() << ": " << ci2 << std::endl;
//        }
        std::cout << "size: " << cell_ids11.size() << std::endl;
      }
  }

/*  // polygon
  std::vector<S2Point> vertices;
  vertices.push_back(S2LatLng::FromDegrees(40.041362, 116.329419).ToPoint());       // 逆时针
  vertices.push_back(S2LatLng::FromDegrees(40.036918, 116.32929).ToPoint());
  vertices.push_back(S2LatLng::FromDegrees(40.036302, 116.33753).ToPoint());
  vertices.push_back(S2LatLng::FromDegrees(40.040959, 116.337884).ToPoint());
  vertices.push_back(S2LatLng::FromDegrees(40.042265, 116.332391).ToPoint());
  std::unique_ptr<S2Loop> loop(new S2Loop(vertices));
  S2Polygon polygon(std::move(loop));
  cell_union = rc.GetCovering(polygon);
  std::cout << "polygon: " << *//*polygon << *//*std::endl;
  for (auto &ci : cell_union) {
      std::cout << ci.level() << ": " << ci.id() << std::endl;
  }*/

  util::units::Meters meters =
          S2Earth::GetDistance(S2LatLng::FromDegrees(40.041362, 116.329419),
                               S2LatLng::FromDegrees(40.036918, 116.32929));
  std::cout << "meters: " << meters << std::endl;

  return  0;
}
