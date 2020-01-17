#pragma once

#include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkInteractionStyle)

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkAxesActor.h>
#include <vtkProperty.h>
#include <vtkOBJReader.h>
#include <vtkTransform.h>
#include<vtkTransformFilter.h>
#include <vtkProp3D.h>
#include <vtkOpenGLCamera.h>

namespace vtk_viz {
	void show_ply(const std::vector<boost::filesystem::path>& path_to_obj);
}