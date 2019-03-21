//
// Created by FyS on 27/06/17.
//

#ifndef FREESOULS_MOCKVERIFIER_HH
#define FREESOULS_MOCKVERIFIER_HH

#include <string>
#include <functional>
#include <map>


namespace FSeam {

    /**
     * \brief basic structure that contains description and utilisation of mocked method
     *
     */
    struct MethodCallVerifier {
        std::string _methodName;
        std::size_t _called = 0;

        std::function<void(void*)> _handler;
    };

    /**
     * \brief Mocking class, it contains all mocked method / save all calls to methods
     * \todo improve the mocking class to take the arguments and compare them in a verify
     */
    class MockClassVerifier {
    public:
        void invokeDupedMethod(std::string &&className, std::string &&methodName, void *arg = nullptr) {
            std::string key = std::move(className) + std::move(methodName);

            if (_verifiers.find(key) != _verifiers.end()) {
                auto dupedMethod = _verifiers.at(key)->_handler;
                if (dupedMethod)
                    dupedMethod(arg);
            }
        }

        /**
         * This method has to be called each time a mocked class is calling a method (in order to register the call)
         *
         * \param className name of the mocked class
         * \param methodName name of the method called
         */
        void methodCall(std::string &&className, std::string &&methodName) {
            auto methodCallVerifier = std::make_shared<MethodCallVerifier>();
            std::string key = std::move(className) + methodName;

            if (_verifiers.find(key) != _verifiers.end())
                methodCallVerifier = _verifiers.at(key);
            methodCallVerifier->_methodName = std::move(methodName);
            methodCallVerifier->_called += 1;
            _verifiers[key] = methodCallVerifier;
        }

        /**
         * \brief This method make it possible to dupe a method in order to have it do what you want.
         *
         * \tparam T handler type
         * \param className name of the class to mock
         * \param methodName method name to dupe
         * \param handler dupped method
         */
        void dupeMethod(std::string &&className, std::string &&methodName, std::function<void(void*)> handler) {
            auto methodCallVerifier = std::make_shared<MethodCallVerifier>();
            std::string key = std::move(className) + methodName;

            if (_verifiers.find(key) != _verifiers.end())
                methodCallVerifier = _verifiers.at(key);
            methodCallVerifier->_methodName = std::move(methodName);
            methodCallVerifier->_called = 0;
            methodCallVerifier->_handler = handler;
            _verifiers[key] = methodCallVerifier;
        }

        /**
         * \brief Verify if a method has been called under certain conditions (number of times)
         *
         * \param className class name to verify
         * \param methodName method to verify
         * \param times number of times you verify that the mocked method has been called, if no value set, this method
         * verify you at least have the mocked method called once
         * \return true if the method encounter your conditions (number of times called), false otherwise
         */
        bool verify(std::string &&className, std::string &&methodName, const int times = -1) const {
            std::string key = std::move(className) + std::move(methodName);

            if (_verifiers.find(key) == _verifiers.end()) {
                if (times > 0)
                    std::cout << "Verify error for method " << key << ", method never have been called while we expected "
                              << times << " calls" << std::endl;
                return times == 0;
            }
            bool result = ((times <= -1 && _verifiers.at(key)->_called > 0) || (_verifiers.at(key)->_called == times));
            if (!result)
                std::cout << "Verify error for method " << key << ", method has been called "
                          << _verifiers.at(key)->_called << " and not " << times << std::endl;
            return result;
        }

    private:
        std::map<std::string, std::shared_ptr<MethodCallVerifier> > _verifiers;
    };

    /**
     * \brief Mocking singleton, this class contains all the mock
     */
    class MockVerifier {
        static std::unique_ptr<MockVerifier> inst;

    public:
        MockVerifier() = default;
        ~MockVerifier() = default;

        static MockVerifier &instance() {
            if (inst == nullptr) {
                inst = std::make_unique<MockVerifier>();
            };
            return *(inst.get());
        }

        static void cleanUp() {
            inst = nullptr;
        }

        /**
         * \brief This method get the mock verifier instance class
         *
         * \details Method that retrieve the mock verifier instance class corresponding to the pointer given as parameter
         * \note A mock verifier instance class is a class that acknowledge all utilisation (method calls) of the mocked class
         * this class also contains the mocked method (dupped).
         *
         * \param mockPtr
         * \return the mock verifier instance class, if not referenced yet, create one by calling the ::addMock(T) method
         */
        std::shared_ptr<MockClassVerifier> &getMock(const void *mockPtr) {
            if (inst->_mockedClass.find(mockPtr) == inst->_mockedClass.end())
                return addMock(mockPtr);
            return inst->_mockedClass.at(mockPtr);
        }

        /**
         * \brief Add a mock verifier instance class into the current testing context
         * \note A mock verifier instance class is a class that acknowledge all utilisation (method calls) of the mocked class
         * this class also contains the mocked method (dupped).
         * \tparam T
         * \param mockPtr
         * \return
         */
        template <typename T>
        std::shared_ptr<MockClassVerifier> &addMock(const T *mockPtr) {
            auto classVerifier = std::make_shared<MockClassVerifier>();
            inst->_mockedClass[mockPtr] = classVerifier;
            return inst->_mockedClass.at(mockPtr);
        }

    private:
        std::map<const void*, std::shared_ptr<MockClassVerifier> > _mockedClass;
    };

    // ------------------------ Helper Method --------------------------


}

#endif //FREESOULS_MOCKVERIFIER_HH
