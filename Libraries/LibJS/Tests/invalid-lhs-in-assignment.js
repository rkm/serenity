load("test-common.js");

try {
    try {
        Math.abs(-20) = 40;
        assertNotReached();
    } catch (e) {
        assert(e.name === "ReferenceError");
        assert(e.message === "Invalid left-hand side in assignment");
    }

    try {
        512 = 256;
        assertNotReached();
    } catch (e) {
        assert(e.name === "ReferenceError");
        assert(e.message === "Invalid left-hand side in assignment");
    }

    try {
        "hello world" = "another thing?";
        assertNotReached();
    } catch (e) {
        assert(e.name === "ReferenceError");
        assert(e.message === "Invalid left-hand side in assignment");
    }

    console.log("PASS");
} catch (e) {
    console.log("FAIL: " + e);
}
