Blockly.Blocks['find_entity'] = {
  init: function() {
    this.appendDummyInput()
        .appendField("Name")
        .appendField(new Blockly.FieldTextInput("default"), "NAME");
    this.setOutput(true, "Entity");
    this.setColour(300);
 this.setTooltip("Find an Entity with the specified name");
 this.setHelpUrl("");
  }
};

Blockly.Blocks['log'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldTextInput("Write here"), "TXT");
    this.setPreviousStatement(true, null);
    this.setNextStatement(true, null);
    this.setColour(225);
 this.setTooltip("Writes to the console");
 this.setHelpUrl("");
  }
};

Blockly.Blocks['start_block'] = {
  init: function() {
    this.appendStatementInput("NAME")
        .setCheck(null)
        .appendField("Start");
    this.setColour(105);
 this.setTooltip("Start gets called after loading a new scene");
 this.setHelpUrl("");
  }
};

Blockly.Blocks['update_block'] = {
  init: function() {
    this.appendStatementInput("NAME")
        .setCheck(null)
        .appendField("Update");
    this.setColour(105);
 this.setTooltip("Start gets called after loading a new scene");
 this.setHelpUrl("");
  }
};