inlets = 5;
outlets = 1;
const VEL_MAX = 127;
const VEL_LOW = 20;

function augment() {
	var inputs   = arrayfromargs(arguments);
	var biased   = inputs[4];
	var sizzlers = inputs.slice(0, 4);
	
	if (biased === 0) {
		outlet(0, sizzlers); 
	}
	else {
	var augmented = sizzlers.map(function (e, i) {
		return e === 0 ? 0 : i === biased - 1 ? VEL_MAX : VEL_LOW;	
	})
	outlet(0, augmented);
	}
}
