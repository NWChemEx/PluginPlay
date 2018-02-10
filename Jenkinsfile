def repo_name="NWChemExRuntime"
def depends = ['NWChemExBase', 'LibChemist']

def compile_repo(depend_name, install_root, do_install) {
    sh """
        set +x
        source /etc/profile
        module load gcc/7.1.0
        module load cmake
        build_tests="True"
        make_command=""
        if [ ${do_install} == "True" ];then
            build_tests="False"
            make_command="install"
        fi
        cmake -H. -Bbuild -DBUILD_TESTS=\${build_tests} \
                          -DCMAKE_INSTALL_PREFIX=${install_root}\
                          -DCMAKE_PREFIX_PATH=${install_root}
        cd build && make \${make_command}
    """
}

node {
    def install_root="${WORKSPACE}/install"
    stage('Build Dependencies') {
        for(int i=0; i<depends.size(); i++) {
            dir("${depends[i]}"){
                git credentialsId:'422b0eed-700d-444d-961c-1e58cc75cda2',
                    url:"https://github.com/NWChemEx-Project/${depends[i]}.git",
                    branch: 'master'
                compile_repo("${depends[i]}", "${install_root}", "True")
            }
        }
    }
    stage('Build Repo') {
        checkout scm
        compile_repo("${repo_name}", "${install_root}", "False")
    }
    stage('Test Repo') {
        sh """
        set +x
        source /etc/profile
        module load cmake
        cd build && ctest
        """
    }
}


