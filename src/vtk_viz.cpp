#include "vtk_vis.h"
	void vtk_viz::show_ply(const std::vector<boost::filesystem::path>& path_to_obj)
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
		vtkSmartPointer<vtkAxesActor> axes =
			vtkSmartPointer<vtkAxesActor>::New();
		renderer->SetBackground(0, 0, 0);
		renderer->UseShadowsOff();
		renderer->AddActor(axes);
		
		double *width ,x=0, d=0;
		width = &d;
		int odin = 1;
		for (int i = 0; i < path_to_obj.size(); i++)
		{
				std::string obj_path;
				obj_path = path_to_obj[i].string();
				const char* path_to_obj_char = obj_path.c_str();
				vtkSmartPointer<vtkOBJReader> reader =
					vtkSmartPointer<vtkOBJReader>::New();
				reader->SetFileName(path_to_obj_char);
				//reader->Update();	

				// Visualizar
				vtkSmartPointer<vtkPolyDataMapper> mapper =
					vtkSmartPointer<vtkPolyDataMapper>::New();
				mapper->SetInputConnection(reader->GetOutputPort());

				vtkSmartPointer<vtkActor> actor =
					vtkSmartPointer<vtkActor>::New();
		
				actor->SetPosition(x, .0, .0);
				actor->SetMapper(mapper);
				if (i == 0)
				{
					actor->GetProperty()->SetColor(0.0, 1.0, 0.0); //(R,G,B)
				}
				else {
					actor->GetProperty()->SetColor(1.0, 0.0, 0.0); //(R,G,B)
				}
				width= actor->GetXRange();
				x = x + *width / 2 + odin;
					renderer->AddActor(actor);
				

		}
		renderWindow->Render();
		renderWindowInteractor->Start();
		vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
			vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); //like paraview

		renderWindowInteractor->SetInteractorStyle(style);

		renderWindowInteractor->Start();
	}
