// This is a demo file demonstrating some capabilities of the
// matching script engine
// This script runs after(!) the matching has been completed
//
// inObject      values as from network
// outObject     to be returned back to matching engine
// matchedObject the result so far, same as inObject for reverse lookup
// modelSet      user's current model set, not available in reverse lookup
// webServices   wraps access to swift web services
//
// outObject.rerun    = true => rerun matching or reverse lookup after the value has been changed again, like a 2nd pass
// outObject.modified = true => tell  matching or reverse lookup that something has been changed
//
(function () {
	// avoid breaking matching/reverse lookup
	try {

        outObject.logMessage = "matching script"; // display log

	} catch (err) {
		return err.toString();
	}

	return outObject;
})
