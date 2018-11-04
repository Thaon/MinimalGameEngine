/* TODO: Change toolbox XML ID if necessary. Can export toolbox XML from Workspace Factory. */
var blocklyDiv = document.getElementById("blocklyDiv");

var options = { 
	toolbox : document.getElementById('toolbox'), 
	collapse : true, 
	comments : true, 
	disable : false, 
	maxBlocks : Infinity, 
	trashcan : true, 
	horizontalLayout : false, 
	toolboxPosition : 'start', 
	css : true, 
	media : 'https://blockly-demo.appspot.com/static/media/', 
	rtl : false, 
	scrollbars : true, 
	sounds : true, 
	oneBasedIndex : true, 
	grid : {
		spacing : 20, 
		length : 1, 
		colour : '#888', 
		snap : true
	}, 
	zoom : {
		controls : true, 
		wheel : true, 
		startScale : 1, 
		maxScale : 3, 
		minScale : 0.3, 
		scaleSpeed : 1.2
	}
};

console.log(blocklyDiv, options);

/* Inject your workspace */ 
var workspace = Blockly.inject(blocklyDiv, options);

console.log(workspace);