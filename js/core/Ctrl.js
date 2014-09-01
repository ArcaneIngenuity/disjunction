Ctrl = function()
{
	this.app = undefined;
	this.model = undefined;

	/** Used to set up resources or values specific to this Ctrl / Model (and thus the entire Phase). */ 
	this.start = function()
	{
		//ABSTRACT: OVERRIDE
	};
	
	/** Used to clean up resources or reset values for this Ctrl, if it is no longer needed and can be released. */ 
	this.finish = function()
	{
		//ABSTRACT: OVERRIDE
	};
	
	/** Update simulation state by making changes to associated Model. */
	this.simulate = function(deltaSec)
	{
		//ABSTRACT: OVERRIDE
		//NOTE input that applies irrespective of View focus, should be run here.
	};
};

