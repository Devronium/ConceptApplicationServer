function avg(out, val) {
	if ((avg._private) && (avg._private.o !== out))
		delete avg._private;
	if (!avg._private)
		avg._private = {"sum": 0, "count": 0, "o": out};

	avg._private.sum += val;
	avg._private.count = avg._private.count + 1;

	return avg._private.sum / avg._private.count;
}

function sum(out, val) {
	if ((sum._private) && (sum._private.o !== out))
		delete sum._private;
	if (!sum._private)
		sum._private = {"sum": 0, "o": out};

	sum._private.sum += val;
	return sum._private.sum;
}

print("Server js functions initialized\n");
