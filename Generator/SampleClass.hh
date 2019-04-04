#include <vector>
#include <string>

#define DEF_1 1
#define OS_NAME "Linux"

using namespace std;
class SampleClass
{
public:
    SampleClass();
    /*!
     * Method 1
     */
    std::string meth1();

    ///
    /// Method 2 description
    ///
    /// @param v1 Variable 1
    ///
    int meth2(int v1);

    /**
     * Method 3 description
     *
     * \param v1 Variable 1
     * \param v2 Variable 2
     */
    void meth3(const std::string& victory, std::vector<std::string> & v2);

    void methodImpl() {}

    /**********************************
     * Method 4 description
     *
     * @return Return value
     *********************************/
    unsigned int meth4();
private:
    void * meth5(){return NULL};

    /// prop1 description
    std::string prop1;
    //! prop5 description
    int prop5;
};
namespace Alpha
{
    class AlphaClass
    {
    public:
        AlphaClass();

        void alphaMethod();
        int alphaMethod2()

        std::string alphaString;
    };

    namespace Omega
    {
        class OmegaClass
        {
        public:
            OmegaClass();

            const std::string &omegaString();

            std::string omegaString;
        };
    };
}

class StaticClassContender {

public:
    static int staticFunction();
};

namespace TESTING {
    int sampleFreeFunction(int i)
    {
        return i + 1;
    }
}

int anotherFreeFunction(void);
}
