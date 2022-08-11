#include "fileFormat.h"

#include "stl_reader.h"

#include <pxr/pxr.h>

#include <pxr/base/tf/diagnostic.h>
#include <pxr/base/tf/stringUtils.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/usdaFileFormat.h>
#include <pxr/usd/usdGeom/mesh.h>

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

#include <iostream>

using namespace std;

PXR_NAMESPACE_OPEN_SCOPE

TF_DEFINE_PUBLIC_TOKENS(UsdStlFileFormatTokens, USD_STL_FILE_FORMAT_TOKENS);

TF_REGISTRY_FUNCTION(TfType) {
  SDF_DEFINE_FILE_FORMAT(UsdStlFileFormat, SdfFileFormat);
}

enum StlCodes { STL_CANNOT_READ_STL_FILE, STL_CANNOT_CREATE_ATTRIBUTE };
TF_REGISTRY_FUNCTION(TfEnum) {
    TF_ADD_ENUM_NAME(STL_CANNOT_READ_STL_FILE, "Cannot read STL file.");
    TF_ADD_ENUM_NAME(STL_CANNOT_CREATE_ATTRIBUTE, "Cannot create attribute.");
};

auto _Convert(const vector<float> &vecs) {
  VtVec3fArray ret(vecs.size() / 3);
  auto pcoords = vecs.data();
  for (auto &v : ret) {
    v.Set(pcoords[0], pcoords[1], pcoords[2]);
    pcoords += 3;
  }
  return ret;
}

UsdStlFileFormat::UsdStlFileFormat()
    : SdfFileFormat(UsdStlFileFormatTokens->Id, UsdStlFileFormatTokens->Version,
                    UsdStlFileFormatTokens->Target,
                    UsdStlFileFormatTokens->Extension) {}

UsdStlFileFormat::~UsdStlFileFormat() {}

bool UsdStlFileFormat::CanRead(const std::string &) const { return true; }

bool UsdStlFileFormat::Read(SdfLayer *layer, const std::string &resolvedPath,
                            bool metadataOnly) const {
  if (!TF_VERIFY(layer)) {
    return false;
  }

  SdfLayerRefPtr newLayer = SdfLayer::CreateAnonymous(".usd");
  UsdStageRefPtr stage = UsdStage::Open(newLayer);

  // Reading the STL

  std::vector<float> coords, normals;
  std::vector<unsigned int> tris, solids;
  try {
    stl_reader::ReadStlFile(resolvedPath.c_str(), coords, normals, tris,
                            solids);
  } catch (std::exception &e) {
    TF_ERROR(STL_CANNOT_READ_STL_FILE, resolvedPath);
    return false;
  }

  // Define the loaded prim

  UsdGeomMesh mesh = UsdGeomMesh::Define(stage, SdfPath("/Root"));
  stage->SetDefaultPrim(mesh.GetPrim());

  // faceVertexCounts

  VtIntArray faceVertexCounts(tris.size() / 3, 3);
  if (!mesh.CreateFaceVertexCountsAttr(VtValue(faceVertexCounts))) {
    TF_ERROR(STL_CANNOT_CREATE_ATTRIBUTE, "faceVertexCounts");
  }

  // faceVertexIndices

  VtIntArray faceVertexIndices(tris.cbegin(), tris.cend());
  if (!mesh.CreateFaceVertexIndicesAttr(VtValue(faceVertexIndices))) {
    TF_ERROR(STL_CANNOT_CREATE_ATTRIBUTE, "faceVertexIndices");
  }

  // points

  if (!mesh.CreatePointsAttr(VtValue(_Convert(coords)))) {
    TF_ERROR(STL_CANNOT_CREATE_ATTRIBUTE, "points");
  }

  // normals

  if(!mesh.CreateNormalsAttr(VtValue(_Convert(normals)))) {
    TF_ERROR(STL_CANNOT_CREATE_ATTRIBUTE, "normals");
  }
  if (!mesh.SetNormalsInterpolation(UsdGeomTokens->uniform)) {
    TF_ERROR(STL_CANNOT_CREATE_ATTRIBUTE, "normalsInterpolation");
  }

  // subdivisionScheme

  if (!mesh.CreateSubdivisionSchemeAttr(VtValue(UsdGeomTokens->none))) {
    TF_ERROR(STL_CANNOT_CREATE_ATTRIBUTE, "subdivisionScheme");
  }

  // transfer the layer

  layer->TransferContent(newLayer);
  return true;
}

bool UsdStlFileFormat::WriteToString(const SdfLayer &layer, std::string *str,
                                     const std::string &comment) const {
  return SdfFileFormat::FindById(UsdUsdaFileFormatTokens->Id)
      ->WriteToString(layer, str, comment);
}

bool UsdStlFileFormat::WriteToStream(const SdfSpecHandle &spec,
                                     std::ostream &out, size_t indent) const {
  return SdfFileFormat::FindById(UsdUsdaFileFormatTokens->Id)
      ->WriteToStream(spec, out, indent);
}

PXR_NAMESPACE_CLOSE_SCOPE
