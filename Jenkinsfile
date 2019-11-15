
pipeline {
    agent { docker { image 'golang' } }
    stages {
        stage('build') {
            steps {
                sh 'clang --version'
                sh 'echo "Test output"'
                
            }
        }
    }
}

