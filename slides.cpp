struct SafeBuffer {
    int* data;
    
    SafeBuffer(int n) : data(new int[n]) {    // Constructor: allocate
        // Resource acquired
    }
    
    ~SafeBuffer() {                           // Destructor: free
        delete[] data;                         // Resource released
    }
};

void function() {
    SafeBuffer buf(100);     // Constructor called - memory allocated
    // ... use buf ...
}   // ← Destructor AUTOMATICALLY called - memory freed! 


