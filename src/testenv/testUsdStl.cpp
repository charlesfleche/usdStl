#include <pxr/base/arch/systemInfo.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/pathUtils.h>
#include <pxr/usd/sdf/path.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/mesh.h>

#include <iostream>
#include <string>

PXR_NAMESPACE_USING_DIRECTIVE
using namespace std;

int main(void) {
  // Register the usdStl plugin

  auto path = ArchGetExecutablePath();
  path += string("/../../usdStl/resources");
  path = TfNormPath(path);

  const auto plugins = PlugRegistry::GetInstance().RegisterPlugins(path);
  if (plugins.empty()) {
    cerr << "Cannot register usdStl from " << path << endl;
    return EXIT_FAILURE;
  }

  // Source stage pointing to an Stl file

  auto stagePath = ArchGetExecutablePath();
  stagePath += string("/../testUsdStl.usda");
  stagePath = TfNormPath(stagePath);

  const auto stage = UsdStage::Open(stagePath);
  if (!stage) {
    cerr << "Cannot open test stage " << stagePath << endl;
    return EXIT_FAILURE;
  }

  // Stage pointing to the expected result

  auto expectedPath = ArchGetExecutablePath();
  expectedPath += string("/../testUsdStlExpected.usda");
  expectedPath = TfNormPath(expectedPath);

  const auto expectedStage = UsdStage::Open(expectedPath);
  if (!expectedStage) {
    cerr << "Cannot open expected stage " << expectedPath << endl;
    return EXIT_FAILURE;
  }

  // Getting the exposed STL mesh

  SdfPath meshPath = SdfPath("/StlCube");
  const auto mesh = UsdGeomMesh::Get(stage, meshPath);
  TF_AXIOM(mesh);
  const auto expectedMesh = UsdGeomMesh::Get(expectedStage, meshPath);
  TF_AXIOM(expectedMesh);

  // faceVertexCounts

  {
    VtIntArray src;
    VtIntArray expected;
    TF_AXIOM(mesh.GetFaceVertexCountsAttr().Get(&src));
    TF_AXIOM(expectedMesh.GetFaceVertexCountsAttr().Get(&expected));
    TF_AXIOM(src == expected);
  }

  // faceVertexIndices

  {
    VtIntArray src;
    VtIntArray expected;
    TF_AXIOM(mesh.GetFaceVertexIndicesAttr().Get(&src));
    TF_AXIOM(expectedMesh.GetFaceVertexIndicesAttr().Get(&expected));
    TF_AXIOM(src == expected);
  }

  // normals

  {
    VtVec3fArray src;
    VtVec3fArray expected;
    TF_AXIOM(mesh.GetNormalsAttr().Get(&src));
    TF_AXIOM(expectedMesh.GetNormalsAttr().Get(&expected));
    TF_AXIOM(src == expected);

    const auto a = mesh.GetNormalsInterpolation();
    const auto b = expectedMesh.GetNormalsInterpolation();
    TF_AXIOM(mesh.GetNormalsInterpolation() ==
             expectedMesh.GetNormalsInterpolation());
  }

  // points

  {
    VtVec3fArray src;
    VtVec3fArray expected;
    TF_AXIOM(mesh.GetPointsAttr().Get(&src));
    TF_AXIOM(expectedMesh.GetPointsAttr().Get(&expected));
    TF_AXIOM(src == expected);
  }

  // subvision scheme

  {
    TfToken src;
    TfToken expected;
    TF_AXIOM(mesh.GetSubdivisionSchemeAttr().Get(&src));
    TF_AXIOM(expectedMesh.GetSubdivisionSchemeAttr().Get(&expected));
    TF_AXIOM(src == expected);
  }

  return EXIT_SUCCESS;
}
