AppModel = function()
{	
	Model.call(this);

	this.serialise = function()
	{
		return serialised;
	}
	
	this.deserialise = function(serialised)
	{
	
	}
};

AppModel.prototype = Object.create(DJ.Core.Model.prototype);
AppModel.prototype.constructor = AppModel;