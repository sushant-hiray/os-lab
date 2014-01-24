bool fexists(char* file){
	if(access(file, F_OK) != -1){
		printf("[in run] %s file exists\n",file);return true;
	} 
	else{
		printf("[in run] %s file doesn't exists\n",file);return false;
	}
}

void run(char* bat_file){
	if(fexists(bat_file)){
		FILE *fp;
		fp = fopen(*bat_file, "r");
		if(fp!=NULL){
			char line[80];
			while(fgets(line, 80, fp) != NULL){
				char *cmd;
				char **argv;
				argv=tokenize(line);
				analyze(argv);
			}
		}
		else{
			printf("file %s didn't open\n", bat_file);
		}
		fclose(fp);
	}
	else{
		printf("file %s doesn't existsn\n", bat_file);
	}
	return;
}