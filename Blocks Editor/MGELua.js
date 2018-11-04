Blockly.Lua['find_entity'] = function(block) {
  var text_name = block.getFieldValue('NAME');
  // TODO: Assemble Lua into code variable.
  var code = 'FindEntity('+text_name+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Lua['log'] = function(block) {
  var text_txt = block.getFieldValue('TXT');
  // TODO: Assemble Lua into code variable.
  var code = 'Log('+text_txt+')';
  // TODO: Change ORDER_NONE to the correct strength.
  return [code, Blockly.Lua.ORDER_NONE];
};

Blockly.Lua['start_block'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'function Start()\n\nend';
  return code;
};

Blockly.Lua['update_block'] = function(block) {
  // TODO: Assemble Lua into code variable.
  var code = 'function Update()\n\nend';
  return code;
};