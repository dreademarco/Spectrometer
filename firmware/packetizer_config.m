
function packetizer_config(this_block)

  % Revision History:
  %
  %   30-Jan-2015  (18:43 hours):
  %     Original code was machine generated by Xilinx's System Generator after parsing
  %     /media/psf/Data/Code/Spectrometer/firmware/packetizer.vhd
  %
  %
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  % Grab HDL parameters from simulink mask
  myname = this_block.blockName; %This is the name of the black box block
  bb_mask = get_param(myname,'Parent'); %This is the name of the black box subsystem
  count_sw = str2num(get_param(bb_mask,'count_sw')); %Grab the parameter from the parent's mask
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  this_block.setTopLevelLanguage('VHDL');

  this_block.setEntityName('packetizer');

  % System Generator has to assume that your entity  has a combinational feed through; 
  %   if it  doesn't, then comment out the following line:
  this_block.tagAsCombinational;

  this_block.addSimulinkInport('reset_in');

  this_block.addSimulinkOutport('reset');
  this_block.addSimulinkOutport('tx_data');
  this_block.addSimulinkOutport('tx_valid');
  this_block.addSimulinkOutport('tx_eof');

  reset_port = this_block.port('reset');
  reset_port.setType('UFix_1_0');
  reset_port.useHDLVector(false);
  
  tx_valid_port = this_block.port('tx_valid');
  tx_valid_port.setType('UFix_1_0');
  tx_valid_port.useHDLVector(false);
  
  tx_eof_port = this_block.port('tx_eof');
  tx_eof_port.setType('UFix_1_0');
  tx_eof_port.useHDLVector(false);
  
  tx_data_port = this_block.port('tx_data');
  tx_data_port.setType('UFix_64_0');
  tx_data_port.useHDLVector(true);

  % -----------------------------
  if (this_block.inputTypesKnown)
    % do input type checking, dynamic output type and generic setup in this code block.

    if (this_block.port('reset_in').width ~= 1);
      this_block.setError('Input data type for port "reset_in" must have width=1.');
    end

    this_block.port('reset_in').useHDLVector(false);

  % (!) Port 'tx_data' appeared to have dynamic type in the HDL
  % --- you must add an appropriate type setting for this port
  end  % if(inputTypesKnown)
  % -----------------------------

  % -----------------------------
   if (this_block.inputRatesKnown)
     setup_as_single_rate(this_block,'clk','ce')
   end  % if(inputRatesKnown)
  % -----------------------------

    % (!) Set the inout port rate to be the same as the first input 
    %     rate. Change the following code if this is untrue.
    uniqueInputRates = unique(this_block.getInputRates);

  % (!) Custimize the following generic settings as appropriate. If any settings depend
  %      on input types, make the settings in the "inputTypesKnown" code block.
  %      The addGeneric function takes  3 parameters, generic name, type and constant value.
  %      Supported types are boolean, real, integer and string.
  if(count_sw == 1)
      this_block.addGeneric('count_sw','STD_LOGIC','''1''');
   else
       this_block.addGeneric('count_sw','STD_LOGIC','''0''');
   end

  % Add addtional source files as needed.
  %  |-------------
  %  | Add files in the order in which they should be compiled.
  %  | If two files "a.vhd" and "b.vhd" contain the entities
  %  | entity_a and entity_b, and entity_a contains a
  %  | component of type entity_b, the correct sequence of
  %  | addFile() calls would be:
  %  |    this_block.addFile('b.vhd');
  %  |    this_block.addFile('a.vhd');
  %  |-------------

  this_block.addFile('counter.vhd');
  this_block.addFile('heap_generator.vhd');
  this_block.addFile('running_generator.vhd');
  this_block.addFile('constant_generator.vhd');
  this_block.addFile('packetizer.vhd');

return;


% ------------------------------------------------------------

function setup_as_single_rate(block,clkname,cename) 
  inputRates = block.inputRates; 
  uniqueInputRates = unique(inputRates); 
  if (length(uniqueInputRates)==1 & uniqueInputRates(1)==Inf) 
    block.addError('The inputs to this block cannot all be constant.'); 
    return; 
  end 
  if (uniqueInputRates(end) == Inf) 
     hasConstantInput = true; 
     uniqueInputRates = uniqueInputRates(1:end-1); 
  end 
  if (length(uniqueInputRates) ~= 1) 
    block.addError('The inputs to this block must run at a single rate.'); 
    return; 
  end 
  theInputRate = uniqueInputRates(1); 
  for i = 1:block.numSimulinkOutports 
     block.outport(i).setRate(theInputRate); 
  end 
  block.addClkCEPair(clkname,cename,theInputRate); 
  return; 

% ------------------------------------------------------------
