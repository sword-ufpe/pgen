.PHONY: clean All

All:
	@echo "----------Building project:[ pgen - Debug ]----------"
	@$(MAKE) -f  "pgen.mk"
clean:
	@echo "----------Cleaning project:[ pgen - Debug ]----------"
	@$(MAKE) -f  "pgen.mk" clean
