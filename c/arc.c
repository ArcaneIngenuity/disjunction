#include "arc.h"

#define ARC_DEBUG_ONEOFFS 1
//#define ARC_DEBUG_UPDATES 1

//--------- Hub ---------//

void Hub_setDefaultCallbacks(Hub * hub)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_setDefaultCallbacks\n");
	#endif
	
	hub->initialise = (void * const)&doNothing;
	hub->dispose 	= (void * const)&doNothing;
	hub->suspend 	= (void * const)&doNothing;
	hub->resume 	= (void * const)&doNothing;
}

void Hub_update(Hub * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] Hub_update...\n");
	#endif
	
	//update apps
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i]; //read from map of values
		if (app->updating)
			App_update(app);
	}
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Hub_update\n");
	#endif
}

Hub * Hub_construct()
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_construct...\n");
	#endif
	
	//TODO ifdef GCC, link destruct() via attr cleanup 
	//#ifdef __GNUC__
	//App * app __attribute__((cleanup (App_destruct))) = malloc(sizeof(App));
	//#else //no auto destructor!
	Hub * hub = calloc(1, sizeof(App));
	//#endif//__GNUC__
	hub->initialise = (void * const)&doNothing;
	hub->dispose 	= (void * const)&doNothing;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_construct\n");
	#endif
	
	return hub;
}

void Hub_destruct(Hub * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_destruct...\n"); 
	#endif// ARC_DEBUG_ONEOFFS
	
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i];
		if (app)
			App_destruct(app);
	}
	
	this->dispose((void *)this);
	
	//this->initialised = false;
	free(this); //hub object is not a pointer!
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_destruct\n"); 
	#endif// ARC_DEBUG_ONEOFFS
}

void Hub_suspend(Hub * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_suspend...");
	#endif// ARC_DEBUG_ONEOFFS
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i];
		if (app)
			App_suspend(app);
	}

	this->suspend((void *)this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_suspend");
	#endif// ARC_DEBUG_ONEOFFS
}

void Hub_resume(Hub * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_resume...");
	#endif// ARC_DEBUG_ONEOFFS
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i];
		if (app)
			App_resume(app);
	}
	
	this->resume((void *)this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_resume");
	#endif// ARC_DEBUG_ONEOFFS
}

App * const Hub_addApp(Hub * const this, App * const app)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_addApp... (app id=%s)\n", app->id);
	#endif
	if (this->appsCount < APPS_MAX)
	{
		this->apps[this->appsCount++] = app;
		app->hub = this;
		//#ifdef ARC_DEBUG_ONEOFFS
		//LOGI("[ARC] App added with ID %s\n", app->id);
		//#endif// ARC_DEBUG_ONEOFFS
		
		#ifdef ARC_DEBUG_ONEOFFS
		LOGI("[ARC] ...Hub_getApp (app id=%s)\n", app->id);
		#endif
		
		return app;
	}
	return NULL;
}

App * const Hub_getApp(Hub * const this, const char * const id)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Hub_getApp... (app id=%s)\n", id);
	#endif
	
	for (int i = 0; i < this->appsCount; i++)
	{
		App * app = this->apps[i];
		if (strcmp(id, app->id) == 0)
		{
			#ifdef ARC_DEBUG_ONEOFFS
			LOGI("[ARC] ...Hub_getApp (app id=%s)\n", id);
			#endif
			
			return app;
		}
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Hub_getApp (app id=%s)\n", id);
	#endif
	
	return NULL;
}

//--------- App ---------//
App * App_construct(const char * id)//App ** app)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_construct...(id=%s)\n", id);
	#endif
	
	//TODO ifdef GCC, link destruct() via attr cleanup 
	//#ifdef __GNUC__
	//App * app __attribute__((cleanup (App_destruct))) = malloc(sizeof(App));
	//#else //no auto destructor!
	App * app = calloc(1, sizeof(App));
	//#endif//__GNUC__
	*app = appEmpty;
	app->id = id;
	app->initialise = (void * const)&doNothing;
	app->dispose 	= (void * const)&doNothing;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_construct (id=%s)\n", id);
	#endif
	
	return app;
}

void App_update(App * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] App_update... (id=%s)\n", this->id);
	#endif
	
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;
	
	Ctrl_update(ctrl); //abstract
	//if (view != NULL) //JIC user turns off the root view by removing it (since this is the enable/disable mechanism)
	//really, we should just exit if either View or Ctrl are null, at App_start()
	if (view->updating)
		View_update(view);
	Ctrl_updatePost(ctrl); //abstract
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...App_update (id=%s)\n", this->id);
	#endif
}

void App_initialise(App * const this)
{
	this->initialise((void *)this);	
	
	Ctrl_initialise(this->ctrl);
	View_initialise(this->view); //initialises all descendants too
}

void App_start(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_start... (id=%s)\n", this->id);
	#endif
	
	if (!this->updating)
	{
		Ctrl * ctrl = this->ctrl;
		View * view = this->view;
		
		if (!this->initialised)
		{
			//TODO make all LOGE, not LOGI!
			
			//for now, check ALL on initialisation (TODO - move to relevant sections and IFDEF DEBUG)
			if(!this->initialise)
			{
				LOGI("[ARC] App_start - Error: missing initialise function.\n"); 
				exit(1);
			}
			
			if(!this->dispose)
			{
				LOGI("[ARC] App_start - Error: missing dispose function.\n"); 
				exit(1);
			}
			
			/*
			if(!this->input)
			{
				LOGI("[ARC] App_start - Error: missing input function.\n"); 
				exit(1);
			}
			
			if(!this->start)
			{
				LOGI("[ARC] App_start - Error: missing start function.\n"); 
				exit(1);
			}
			
			if(!this->stop)
			{
				LOGI("[ARC] App_start - Error: missing stop function.\n"); 
				exit(1);
			}
			*/
			if(!this->initialise)
			{
				LOGI("[ARC] App_start - Error: missing initialise function.\n"); 
				exit(1);
			}
			
			if(!this->dispose)
			{
				LOGI("[ARC] App_start - Error: missing dispose function.\n"); 
				exit(1);
			}
			
			if(!this->model)
			{
				LOGI("[ARC] App_start - Error: missing model.\n"); 
				exit(1);
			}
			
			if(!this->view)
			{
				LOGI("[ARC] App_start - Error: missing view.\n"); 
				exit(1);
			}
			else
			{
				if (!this->view->start)
				{
					LOGI("[ARC] App_start - Error: missing view start function.\n"); 
					exit(1);
				}
				
				if (!this->view->stop)
				{
					LOGI("[ARC] App_start - Error: missing view stop function.\n"); 
					exit(1);
				}
				
				if (!this->view->initialise)
				{
					LOGI("[ARC] App_start - Error: missing view initialise function.\n"); 
					exit(1);
				}
				
				if (!this->view->dispose)
				{
					LOGI("[ARC] App_start - Error: missing view dispose function.\n"); 
					exit(1);
				}
				
				if (!this->view->update)
				{
					LOGI("[ARC] App_start - Error: missing view update function.\n"); 
					exit(1);
				}
				
				if (!this->view->updatePost)
				{
					LOGI("[ARC] App_start - Error: missing view updatePost function.\n"); 
					exit(1);
				}
				
				//TODO recurse view children.
			}
			
			if(!this->ctrl)
			{
				LOGI("[ARC] App_start - Error: missing ctrl.\n"); 
				exit(1);
			}
			else
			{
				if (!this->ctrl->mustStart)
				{
					LOGI("[ARC] App_start - Error: missing ctrl mustStart function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->mustStop)
				{
					LOGI("[ARC] App_start - Error: missing ctrl mustStop function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->start)
				{
					LOGI("[ARC] App_start - Error: missing ctrl start function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->stop)
				{
					LOGI("[ARC] App_start - Error: missing ctrl stop function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->initialise)
				{
					LOGI("[ARC] App_start - Error: missing ctrl initialise function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->dispose)
				{
					LOGI("[ARC] App_start - Error: missing ctrl dispose function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->update)
				{
					LOGI("[ARC] App_start - Error: missing ctrl update function.\n"); 
					exit(1);
				}
				
				if (!this->ctrl->updatePost)
				{
					LOGI("[ARC] App_start - Error: missing ctrl updatePost function.\n"); 
					exit(1);
				}
			}
			
			//initialise app
			//this->initialise((void *)this);
		}
		
		this->updating = true;
		/*
		if (!ctrl->initialised)
		{
			LOGI("[ARC] App_start - Error: ctrl has not been initialised.\n"); 
			exit(1);
		}
		
		if (!view->initialised)
		{
			LOGI("[ARC] App_start - Error: view has not been initialised.\n"); 
			exit(1);
		}
		*/
		
		Ctrl_start(ctrl);
		View_start(view);
		ctrl->start((void *)ctrl);
		view->start((void *)view);
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_start (id=%s)\n", this->id);
	#endif
}

void App_stop(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_stop... (id=%s)\n", this->id);
	#endif
	
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;
	
	ctrl->stop((void *)ctrl);
	view->stop((void *)view);
	
	this->updating = false;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_stop (id=%s)\n", this->id);
	#endif
}

void App_destruct(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	const char * id = this->id;
	LOGI("[ARC] App_destruct... (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
	
	Ctrl * ctrl = this->ctrl;
	View * view = this->view;

	Ctrl_destruct(ctrl);
	View_destruct(view);
	
	//this->services.dispose();
	
	this->dispose((void *)this);
	this->initialised = false;
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_destruct (id=%s)\n", id);
	#endif//ARC_DEBUG_ONEOFFS
}

void App_suspend(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_suspend... (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
	
	View_suspend(this->view);
	
	this->ctrl->suspend((void *)this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_suspend (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
}

void App_resume(App * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] App_resume... (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
	
	View_resume(this->view);

	this->ctrl->resume((void *)this);

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...App_resume (id=%s)\n", this->id);
	#endif//ARC_DEBUG_ONEOFFS
}

void App_setCtrl(App * app, Ctrl * ctrl)
{
	app->ctrl = ctrl;
	ctrl->app = app;
	ctrl->hub = app->hub;
}

void App_setView(App * app, View * view)
{
	app->view = view;
	view->app = app;
	view->hub = app->hub;
}

//--------- Ctrl ---------//
Ctrl * Ctrl_construct(const char * id, size_t sizeofSubclass)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_construct...(id=%s)\n", id);
	#endif
	
	//since we can't pass in a type,
	//allocate full size of the "subclass" - this is fine as "base"
	//struct is situated from zero in this allocated space
	Ctrl * ctrl = calloc(1, sizeofSubclass);
	ctrl->id = id;
	Ctrl_setDefaultCallbacks(ctrl);
	
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_construct(id=%s)\n", id);
	#endif
	
	return ctrl;
}

void Ctrl_setDefaultCallbacks(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_setDefaultCallbacks (id=%s)\n", this->id);
	#endif
	
	this->mustStart = (void * const)&doNothing;
	this->mustStop 	= (void * const)&doNothing;
	this->start 	= (void * const)&doNothing;
	this->stop 		= (void * const)&doNothing;
	this->suspend 	= (void * const)&doNothing;
	this->resume 	= (void * const)&doNothing;
	this->initialise= (void * const)&doNothing;
	this->dispose 	= (void * const)&doNothing;
	this->update 	= (void * const)&doNothing;
	this->updatePost= (void * const)&doNothing;
}

void Ctrl_start(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_start... (id=%s)\n", this->id);
	#endif
	
	this->start((void *)this);
	this->updating = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_start (id=%s)\n", this->id);
	#endif
}

void Ctrl_stop(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_stop... (id=%s)\n", this->id);
	#endif
	
	this->stop((void *)this);
	this->updating = false;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_stop (id=%s)\n", this->id);
	#endif
}

void Ctrl_suspend(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_suspend... (id=%s)\n", this->id);
	#endif
	
	this->suspend(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_suspend (id=%s)\n", this->id);
	#endif
}

void Ctrl_resume(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_resume... (id=%s)\n", this->id);
	#endif
	
	this->resume(this);	
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_resume (id=%s)\n", this->id);
	#endif
}

void Ctrl_initialise(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] Ctrl_initialise... (id=%s)\n", this->id);
	#endif
	
	this->initialise(this);
	
	this->initialised = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_initialise (id=%s)\n", this->id);
	#endif
}

void Ctrl_update(Ctrl * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] Ctrl_update... (id=%s)\n", this->id);
	#endif
	
	this->update(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Ctrl_update (id=%s)\n", this->id);
	#endif
}

void Ctrl_updatePost(Ctrl * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] Ctrl_updatePost... (id=%s)\n", this->id);
	#endif
	
	this->updatePost(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...Ctrl_updatePost (id=%s)\n", this->id);
	#endif
}

void Ctrl_destruct(Ctrl * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	const char * id = this->id;
	LOGI("[ARC] Ctrl_destruct... (id=%s)\n", id);
	#endif
	
	this->dispose(this);
	this->initialised = false;
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...Ctrl_destruct (id=%s)\n", id);
	#endif
}

//--------- View ---------//
void View_setDefaultCallbacks(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_setDefaultCallbacks (id=%s)\n", this->id);
	#endif
	
	//null id
	//null model
	this->start 			= (void * const)&doNothing;
	this->stop 				= (void * const)&doNothing;
	this->suspend 			= (void * const)&doNothing;
	this->resume 			= (void * const)&doNothing;
	this->initialise		= (void * const)&doNothing;
	this->dispose 			= (void * const)&doNothing;
	this->update 			= (void * const)&doNothing;
	this->updatePost		= (void * const)&doNothing;
	this->onParentResize 	= (void * const)&doNothing;
}

View * View_construct(const char * id, size_t sizeofSubclass)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_construct... (id=%s)\n", id);
	#endif
	
	//since we can't pass in a type,
	//allocate full size of the "subclass" - this is fine as "base"
	//struct is situated from zero in this allocated space
	View * view = calloc(1, sizeofSubclass);
	view->id = id;
	View_setDefaultCallbacks(view);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_construct (id=%s)\n", id);
	#endif
	
	return view;
}

void View_start(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_start... (id=%s)\n", this->id);
	#endif
	
	this->start((void *)this);
	this->updating = true;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_start (id=%s)\n", this->id);
	#endif
}

void View_stop(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_stop... (id=%s)\n", this->id);
	#endif
	
	this->stop((void *)this);
	this->updating = false;
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_stop (id=%s)\n", this->id);
	#endif
}

void View_suspend(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_suspend... (id=%s)\n", this->id);
	#endif
	
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i]; //NB! dispose in draw order
		View_suspend(child);
	}
	
	this->suspend(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_suspend (id=%s)\n", this->id);
	#endif
}

void View_resume(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_resume... (id=%s)\n", this->id);
	#endif
	
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i]; //NB! dispose in draw order
		View_resume(child);
	}
	
	this->resume(this);	
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_resume (id=%s)\n", this->id);
	#endif
}


void View_initialise(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_initialise... (id=%s)\n", this->id);
	#endif
	
	this->initialise(this);
	
	this->initialised = true;

	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i];
		View_initialise(child);//deltaSec //only works if enabled
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_initialise (id=%s)\n", this->id);
	#endif
}

void View_update(View * const this)
{
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] View_update... (id=%s)\n", this->id);
	#endif
	
	this->update(this);//deltaSec

	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i];
		if (child->updating)
			View_update(child);//deltaSec
	}
	
	this->updatePost(this);//deltaSec
	
	#ifdef ARC_DEBUG_UPDATES
	LOGI("[ARC] ...View_update (id=%s)\n", this->id);
	#endif
}

void View_destruct(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	const char * id = this->id;
	LOGI("[ARC] View_destruct... (id=%s)\n", id);
	#endif
	
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i]; //NB! dispose in draw order
		View_destruct(child);
	}
	this->dispose(this);
	this->initialised = false;
	free(this);
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_destruct (id=%s)\n", id);
	#endif
}

View * View_getChild(View * const this, char * id)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_getChild... (id=%s) (child id=%s)\n", this->id, id);
	#endif
	
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i];
		if (strcmp(id, child->id) == 0)
		{
			#ifdef ARC_DEBUG_ONEOFFS
			LOGI("[ARC] ...View_getChild (id=%s) (child id=%s)\n", this->id, id);
			#endif
			
			return child;
		}
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_getChild (id=%s) (child id=%s)\n", this->id, id);
	#endif
	
	return NULL;
}

View * View_addChild(View * const this, View * const child)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_addChild... (id=%s) (child id=%s)\n", this->id, child->id);
	#endif
	
	child->parent = this;
	if (this->root)
		child->root = this->root;
	else
		child->root = this;

	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_addChild (id=%s) (child id=%s)\n", this->id, child->id);
	#endif
	
	if (this->childrenCount == VIEW_CHILDREN_MAX)
		return NULL;
	else
	{
		this->childrenByZ[this->childrenCount++] = child;
		return child;
	}
}

/*
View *
View_removeChild(View * const this, View * const child)
{
	child->parent = NULL;

	if (this->childrenCount == 0)
		return ARRAY_EMPTY;
	else
	{
		//TODO find child index
		
		//TODO shift all back
		
		this->childrenCount--;
		return child;
	}
}
*/
/*
ArrayResult
View_swapChildren(View * const this, int indexFrom, int indexTo)
{
	
}
*/

bool View_isRoot(View * const this)
{
	return this->parent == NULL;
}

void View_onParentResize(View * const this)
{
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] View_onParentResize... (id=%s)\n", this->id);
	#endif
	
	this->onParentResize(this);
	
	for (int i = 0; i < this->childrenCount; i++)
	{
		View * child = (View *) this->childrenByZ[i];
		
		//depth first - update child and then recurse to its children
		
		View_onParentResize(child);
	}
	
	#ifdef ARC_DEBUG_ONEOFFS
	LOGI("[ARC] ...View_onParentResize (id=%s)\n", this->id);
	#endif
}

//--------- misc ---------//
void doNothing(void * const this){/*LOGI("[ARC] doNothing\n");*/}