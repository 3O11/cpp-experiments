#include <iostream>

#include <jni.h>

int main()
{
    std::cout << "Hello World!\n";
    
    JavaVM* jvm;
    JNIEnv* env;

    JavaVMInitArgs vm_args;
    JavaVMOption options[3];

    // Very ugly, will fix later
    options[0].optionString = (char *) "-Djava.library.path=D:\\code\\projects\\cpp-experiments\\build\\_deps\\openjdk-src\\bin";
    options[1].optionString = (char *) "-Djava.class.path=D:\\code\\projects\\cpp-experiments\\build\\10-BundleJava";
    options[2].optionString = (char *) "-verbose:jni";
    vm_args.version = JNI_VERSION_20;
    vm_args.nOptions = 3;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = false;

    JNI_CreateJavaVM(&jvm, reinterpret_cast<void**>(&env), &vm_args);
    if (!jvm) {
        std::cout << "sus\n";
    }

    jvm->DestroyJavaVM();

    std::cout << "Hello World!\n";
}
