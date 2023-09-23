
// In-place, fixed length array.
namespace tk {
    template<typename T, uint32_t CAPACITY>
    struct ArrayN {

        T _elems[CAPACITY];
        uint32_t _top = 0;

        // Returns the length of the array
        uint32_t Length() const {
            return _top;
        }

        T Pop() {
            uint32_t ndx = _top;
            _top--;
            return _elems[ndx];

        }

        uint32_t Capacity() const {
            return CAPACITY;
        }

        void Clear() {
            _top = 0;
        }

        void ClearZero() {
            _top = 0;
            memset( _elems, 0, sizeof(T) * CAPACITY );
        }

        int Add( const T &item ) {
            // TODO: assert (_top < CAPACITY)

            uint32_t ndx = _top;
            _elems[_top++] = item;
            return ndx;
        }

        // Swaps item at index with last index
        void RemoveSwap( int index) {
            _top -= 1;
            _elems[index] = _elems[_top];
        }

        T& operator[]( int index ) {
            return _elems[index];
        }

        T& operator[]( int index ) const {
            return _elems[index];
        }

    }; // struct ArrayN
}