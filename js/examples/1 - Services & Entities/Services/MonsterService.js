//Assists us in retrieving a random, transparent background, Gravatar monster image for use in MainView.
MonsterService = function()
{
	var index = 0;
	
	this.generateIndex = function()
	{
		return index++;
	}
	
	this.generateName = function(index)
	{
		return "a"+index;
	}
	
	this.getImageURL = function(name)
	{
		var md5Hash = md5(name);
		return "http://www.gravatar.com/avatar/"+md5Hash+"?d=monsterid";
	}
}