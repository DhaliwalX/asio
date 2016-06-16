#include <iosfwd>
#include <iostream>
#include <boost/iostreams/categories.hpp>
#include <algorithm>
#include <string>
#include <cassert>
#include <boost/iostreams/stream.hpp>

namespace boost { namespace iostreams { namespace example {
template <typename Container>
class my_source {
public:
    using char_type = char;
    using category = source_tag;

    my_source(Container &container)
        : container_(container), pos_(0)
    { }


    std::streamsize read(char *s, std::streamsize n)
    {
        // read up to n characters from the underlying data source
        // into the buffers, returning the number fo characters
        // read; return -1 to indicate EOF
        using namespace std;
        streamsize amt = static_cast<streamsize>(container_.size() - pos_);
        streamsize result = (min)(n, amt);
        if (result != 0) {
            std::copy(container_.begin() + pos_,
                    container_.begin() + pos_ + result,
                    s);
            pos_ += result;
            return result;
        } else {
            return -1; // EOF
        }
    }
    
    Container &container() { return container_; }
private:
    typedef typename Container::size_type size_type;
    Container &container_;
    size_type pos_;
};

}}}

namespace io = boost::iostreams;
namespace ex = boost::iostreams::example;

int main()
{
    using namespace std;
    typedef ex::my_source<string> string_source;
    string input = "Hello, World";
    string output;
    io::stream<string_source> in(input);
    getline(in, output);
    assert(input == output);
    cout << output << std::endl;
    return 0;
}
