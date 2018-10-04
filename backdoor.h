
String const pattern = "abbaaabbbbbaaaaaaabbbbbbbbbbb";

String secuence = "";

boolean checkBackDoor(boolean state1, boolean state2) {
	boolean success = false;

	if (state1 == true) {
		secuence += "a";
	} 

	if (state2 == true) {
		secuence += "b";
	} 

	if(secuence == pattern) {
		Serial.println("PERFECT");
		success = true;
	} else if(pattern.startsWith(secuence)) {
		Serial.println("good way");
	} else {
		Serial.println("wrong");
		secuence = "";
	}

	return success;
}
