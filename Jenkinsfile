def buildModuleMatrix = [
    		   "GCC 7.1.0":("cmake gcc/7.1.0")
		  ]
node{
    def nwxJenkins
    stage('Import Jenkins Commands'){
        sh """
           da_url=https://raw.githubusercontent.com/NWChemEx-Project/
           da_url+=DeveloperTools/master/ci/Jenkins/nwxJenkins.groovy
           wget \${da_url}
           """
    	nwxJenkins=load("nwxJenkins.groovy")
    }
    nwxJenkins.commonSteps(buildModuleMatrix, "SDE")
}
