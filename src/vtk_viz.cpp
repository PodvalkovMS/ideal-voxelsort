#include "vtk_vis.h"

void show_ply(std::vector<std::string> path_to_obj)
{
	using namespace boost::filesystem;

	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	vtkNew<vtkOpenGLCamera> cam;
	renderer->SetActiveCamera(cam);
	renderer->SetBackground(.2, .3, .4);

	for (int i = 0; i < path_to_obj.size(); i++)
	{
		path Pathobj(path_to_obj[i]);

		if (Pathobj.extension() != ".obj") {
			cout << "Cant read obj " << Pathobj.string() << endl;
			continue;
		}
		else
		{
			cout << i << endl;
			const char* pathToObjChar = path_to_obj[i].c_str();
			vtkSmartPointer<vtkOBJReader> reader =
				vtkSmartPointer<vtkOBJReader>::New();
			reader->SetFileName(pathToObjChar);
			reader->Update();
			reader->GetOutput();

			vtkSmartPointer<vtkTransform> transform =
				vtkSmartPointer<vtkTransform>::New();
			transform->Scale(.005, .005, .005);

			vtkSmartPointer<vtkTransformFilter> transformFilter =
				vtkSmartPointer<vtkTransformFilter>::New();
			transformFilter->SetInputConnection(reader->GetOutputPort());
			transformFilter->SetTransform(transform);

			// Visualizar
			vtkSmartPointer<vtkPolyDataMapper> mapper =
				vtkSmartPointer<vtkPolyDataMapper>::New();
			mapper->SetInputConnection(reader->GetOutputPort());

			vtkSmartPointer<vtkActor> actor =
				vtkSmartPointer<vtkActor>::New();
			actor->SetPosition(1.1 + i, .5, .1);
			actor->SetMapper(mapper);
			vtkSmartPointer<vtkAxesActor> axes =
				vtkSmartPointer<vtkAxesActor>::New();
			renderer->AddActor(actor);
			renderer->SetBackground(0, 0, 0);
			renderer->UseShadowsOff();
			if (i == 1) {
				renderer->AddActor(axes);
			}
		}
	}
	renderWindow->Render();
	renderWindowInteractor->Start();
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); //like paraview

	renderWindowInteractor->SetInteractorStyle(style);

	renderWindowInteractor->Start();
}