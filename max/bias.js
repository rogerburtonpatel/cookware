inlets = 5;
outlets = 2;
const VEL_MAX = 127;
const VEL_LOW = 20;

const Pad_out_region = {
  pad_centercode: 0,
  pad_sizzler_1: 1, pad_sizzler_2: 2,  pad_sizzler_3: 3, pad_sizzler_4: 4,
  pad_hicenter: 5,  pad_leftcenter: 6, pad_locenter: 7,  pad_rightcenter: 8
};

function augment() {
	var inputs   = arrayfromargs(arguments);
	var biased   = inputs[4];
	var sizzlers = inputs.slice(0, 4);
	
	if (biased === 9) {
		outlet(0, sizzlers); 
	}
	else {
	const biasedlist = [];
	/* easier than more switch cases */
	if (biased >= 1 && biased <= 4) {
		biasedlist[0] = biased;
	} else {
		switch (biased) {
			case Pad_out_region.pad_hicenter: 
				biasedlist.push(1);
				biasedlist.push(2);
				break;
			case Pad_out_region.pad_leftcenter: 
				biasedlist.push(2);
				biasedlist.push(3);
				break;
			case Pad_out_region.pad_locenter: 
				biasedlist.push(3);
				biasedlist.push(4);
				break;
			case Pad_out_region.pad_rightcenter: 
				biasedlist.push(4);
				biasedlist.push(1);
				break;
		default: /* 0 */
			break;
		}
	}
		
	const augmenter = function (elem, idx) {
		return elem === 0 ? 0 : (biasedlist.indexOf(idx + 1) >= 0) ? VEL_MAX : VEL_LOW;	
	};
	
	
	var augmented = sizzlers.map(augmenter);
	outlet(0, augmented);
	}
}
