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
		
		double *width_x=0, *wid=0 ,x=0, d=0, *width_first_y=0, *width_y=0, *width_first_z=0, * width_z=0 ;
		
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
				
				actor->SetMapper(mapper);
				if (i == 0)
				{
					actor->GetProperty()->SetColor(0.0, 1.0, 0.0); //(R,G,B)
					width_first_y = actor->GetYRange();
					width_first_z = actor->GetZRange();
					actor->SetPosition(.0, .0, .0);
				}
				else {
				actor->GetProperty()->SetColor(1.0, 0.0, 0.0); 
				width_y = actor->GetYRange();
				width_z = actor->GetZRange();
				*width_y = *width_first_y - *width_y;
				*width_z = *width_first_z - *width_z;
				actor->SetPosition(x, *width_y, *width_z);//(R,G,B)
				}
				width_x= actor->GetXRange();
				d= x - *width_x;
				x = x + d +odin;
				renderer->AddActor(actor);
		}
		renderWindow->Render();
		renderWindowInteractor->Start();
		vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
			vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New(); //like paraview

		renderWindowInteractor->SetInteractorStyle(style);

		renderWindowInteractor->Start();
	}
