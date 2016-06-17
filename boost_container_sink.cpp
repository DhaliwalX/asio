#include <iosfwd>
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>

namespace io = boost::iostreams;

namespace boost { namespace iostreams { namespace example {

    template <typename C>
    class container_sink {
    public:
        using char_type = typename C::value_type;
        using category = sink_tag;
        container_sink(C &container) : container_(container)
        { }

        std::streamsize write(const char_type *s, std::streamsize n)
        {
            container_.insert(container_.end(), s, s + n);
            return n;
        }

        C &container() { return container_; }

    private:
        C &container_;
    };

}}}

namespace ex = io::example;

int main()
{
    using namespace std;
    using string_sink = ex::container_sink<string>;

    string result;
    io::stream<string_sink> out(result);
    out << "Hello, World!";
    out.flush();
    assert(result == "Hello, World!");
}

