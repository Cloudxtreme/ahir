#ifndef _Aa_Expression__
#define _Aa_Expression__

#include <AaIncludes.h>
#include <AaEnums.h>
#include <AaUtil.h>
#include <AaRoot.h>
#include <AaScope.h>
#include <AaType.h>
#include <AaValue.h>


//**************************************** EXPRESSION ****************************************
class AaStorageObject;
class AaStatement;
class AaExpression: public AaRoot
{
  // the containing scope of this expression
  AaScope* _scope;
  bool _coalesce_flag;
  bool _is_target;
  bool _does_pipe_access;
  bool _is_malformed;

 protected:
  // type of the expression
  AaType* _type;

  // expressions which are targets of this
  // expression.
  set<AaExpression*> _targets;

  // coalesce flag, used during memory space
  // identification..
  void Set_Coalesce_Flag(bool v) { _coalesce_flag = v;}
  bool Get_Coalesce_Flag() {return(_coalesce_flag);}


  // the expression, when evaluated at run
  // time, can produce a value.  In the following
  // set, we keep all the storage objects whose
  // addresses can be generated by this expression.
  // we keep one representative of all such 
  // objects (others are connected to this representative
  // in AaProgram::_storage_dependency_graph).
  AaStorageObject* _addressed_object_representative;


  // if this expression is a target through an
  // an assignment statement, then rhs_source
  // is the expression which leads to this..
  set<AaExpression*> _rhs_sources;

  // statement with which it is associated,
  // either as a source or as a target.
  AaStatement* _associated_statement;

  bool _already_evaluated;

  
 public:

  AaValue* _expression_value;


  virtual AaScope* Get_Scope() { return(this->_scope);}

  AaExpression(AaScope* scope_tpr);
  ~AaExpression();
  virtual string Kind() {return("AaExpression");}

  void Set_Associated_Statement(AaStatement* stmt)
  {
    _associated_statement = stmt;
  }
  AaStatement* Get_Associated_Statement()
  {
    return(_associated_statement);
  }
  // malformed if something wrong.. e.g. unknown memory space in pointer deref.
  void Set_Is_Malformed(bool v) { _is_malformed = v;}
  bool Get_Is_Malformed() {return(_is_malformed);}


  void Set_Does_Pipe_Access(bool v) { _does_pipe_access = v; }
  bool Get_Does_Pipe_Access() { return(_does_pipe_access); }

  void Set_Is_Target(bool v) {_is_target = v;}
  bool Get_Is_Target() {return(_is_target);}

  virtual bool Is_Foreign_Store() {return false;}
  virtual bool Is_Foreign_Load() {return false;}

  void Add_RHS_Source(AaExpression* src) { _rhs_sources.insert(src); }
  bool Is_RHS_Source(AaExpression* src) { return(_rhs_sources.find(src) != _rhs_sources.end()); }

  virtual bool Set_Addressed_Object_Representative(AaStorageObject* obj);
  AaStorageObject* Get_Addressed_Object_Representative() 
  {
    return(this->_addressed_object_representative);
  }

  virtual void Propagate_Addressed_Object_Representative(AaStorageObject* obj, AaRoot* from_expr);

  virtual void Set_Type(AaType* t);
  virtual AaType* Get_Type() {return(this->_type);}
  virtual AaValue* Get_Expression_Value() {return(this->_expression_value);}

  virtual void Map_Source_References(set<AaRoot*>& source_objects) { assert(0); }
  virtual void Map_Source_References_As_Target(set<AaRoot*>& source_objects) { assert(0); }
  virtual bool Is_Expression() {return(true); }
  virtual bool Is_Object_Reference() {return(false);}

  virtual void PrintC(ofstream& ofile, string tab_string) { assert(0); }

  virtual AaRoot* Get_Root_Object() {return(NULL);}
  virtual void Update_Type() {};

  virtual void Add_Target(AaExpression* expr) {this->_targets.insert(expr);}
  
  virtual string Get_VC_Name();

  virtual void Write_VC_Control_Path( ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target( ostream& ofile) 
  {
    this->Write_VC_Control_Path(ofile);
  }

  void Write_VC_WAR_Dependencies(set<AaRoot*>& visited_elements,
				 AaExpression* source_expr,
				 ostream& ofile);
  virtual bool Is_Load() {return(false);}
  virtual bool Is_Store() {return(false);}
  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile) {}
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile) {}

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile)
  {
  }
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile)
  {
  }

  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile) {}
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile) {}
  virtual void Write_VC_Wire_Declarations_As_Target(ostream& ofile) {}
  virtual void Write_VC_Datapath_Instances_As_Target( ostream& ofile, AaExpression* source) {}
  virtual void Write_VC_Datapath_Instances(AaExpression* target,  ostream& ofile) {}
  virtual void Write_VC_Links(string hier_id, ostream& ofile) {}
  virtual void Write_VC_Links_As_Target(string hier_id, ostream& ofile) {}
  virtual string Get_VC_Wire_Name()
  {
    return(this->Get_VC_Name() + "_wire");
  }

  virtual string Get_VC_Request_Region_Name()
  {
    return(this->Get_VC_Name() + "_request");
  }

  virtual string Get_VC_Complete_Region_Name()
  {
    return(this->Get_VC_Name() + "_complete");
  }


  virtual string Get_VC_Constant_Name()
  {
    return(this->Get_VC_Wire_Name() + "_constant");
  }

  virtual string Get_VC_Driver_Name()
  {
    if(this->Is_Constant())
      return(this->Get_VC_Constant_Name());
    else
      return(this->Get_VC_Wire_Name());
  }

  virtual string Get_VC_Resized_Base_Address_Name()
  {
    return(this->Get_VC_Name() + "_resized_base_address");
  }

  virtual string Get_VC_Receiver_Name()
  {
    return(this->Get_VC_Wire_Name());
  }

  virtual string Get_VC_Datapath_Instance_Name()
  {
    return(this->Get_VC_Name() +  "_inst");
  }

  virtual void Get_Leaf_Expression_Set(set<AaExpression*>& leaf_expression_set)
  {
    assert(0);
  }

  virtual bool Is_Constant() {return(this->_expression_value != NULL);}

  virtual bool Is_Implicit_Variable_Reference() {return(false);}

  virtual bool Is_Implicit_Object() {return(false);}

  virtual void Evaluate() {if(!_already_evaluated) _already_evaluated = true;};
  virtual void Assign_Expression_Value(AaValue* expr_value);

  virtual bool Scalar_Types_Only() { return(true);}

  set<AaExpression*>& Get_Targets() {return _targets; }

  virtual int Get_VC_Memory_Space_Index() {return (-1);}  // default.. no memory space

  // return true if the only place this is (eventually) used
  // is an address-of expression
  bool Used_Only_In_Address_Of_Expression();
};


// Object Reference: base class for
// references to objects such as a, a[i], ./a[i][j]
// etc.
class AaObjectReference: public AaExpression
{

 protected:


  // the original string which was 
  // in the source file.
  string _object_ref_string;

  // make it more interesting for second pass
  // fixup

  // 0 means start searching from here
  // 1 means start searching in parent of this
  // and so on
  unsigned int _search_ancestor_level; 

  // if vector is (a b), this means that
  // look for child from search start point/a/b
  vector<string> _hier_ids;

  // root object.
  string _object_root_name;

  // the object to which this reference points!
  // figure it out in the second pass.
  // for an object reference which defines
  // an implicit variable, _object points
  // to the statement which defined it.
  AaRoot* _object;

  bool _is_dereferenced;

 public:
  AaObjectReference(AaScope* scope_tpr, string object_ref_string);
  ~AaObjectReference();
  virtual void Print(ostream& ofile);
  virtual string Get_Object_Ref_String() {return(this->_object_ref_string);}
  AaType* Get_Address_Type(vector<AaExpression*>* );

  virtual void Set_Object(AaRoot* obj) {this->_object = obj;}
  virtual AaRoot* Get_Object() { return(this->_object);}
  AaType* Get_Object_Type();
  virtual int Evaluate(vector<AaExpression*>* indices, vector<int>* scale_factors, vector<int>* shift_factors);
  virtual void Add_Hier_Id(string hier_id) {this->_hier_ids.push_back(hier_id);}
  virtual void Set_Object_Root_Name(string orn) {this->_object_root_name = orn; }
  virtual string Get_Object_Root_Name() {return(this->_object_root_name);}
  virtual void Set_Search_Ancestor_Level(unsigned int sal) {this->_search_ancestor_level = sal;}
  virtual unsigned int Get_Search_Ancestor_Level() {return(this->_search_ancestor_level);}
  virtual bool Is_Hierarchical_Reference() 
  {
    return((this->_hier_ids.size() > 0) || (this->_search_ancestor_level > 0));
  }
  virtual string Kind() {return("AaObjectReference");}
  virtual void Map_Source_References(set<AaRoot*>& source_objects); // important
  virtual void Map_Source_References_As_Target(set<AaRoot*>& source_objects)
  {
  }

  virtual void Add_Target_Reference(AaRoot* referrer); 
  virtual void Add_Source_Reference(AaRoot* referrer);
  virtual void PrintC(ofstream& ofile, string tab_string);
  virtual bool Is_Object_Reference() {return(true);}

  virtual void Evaluate();
  void PrintAddressOfC(ofstream& ofile, string tab_string) {}
  virtual void Print_BaseStructRef_C(ofstream& ofile, string tab_string) {}

  virtual void Propagate_Addressed_Object_Representative(AaStorageObject* obj, AaRoot* from_expr);


  // common operations across loads/stores.
  void Write_VC_Load_Control_Path(vector<AaExpression*>* address_expressions,
				  vector<int>* scale_factors,
				  vector<int>* shift_factors,
				  ostream& ofile);
  
  void Write_VC_Store_Control_Path( vector<AaExpression*>* address_expressions,
				    vector<int>* scale_factors,
				    vector<int>* shift_factors,
				    ostream& ofile);

  void Write_VC_Load_Store_Constants(vector<AaExpression*>* address_expressions,
				     vector<int>* scale_factors,
				     vector<int>* shift_factors,
				     ostream& ofile);

  void Write_VC_Load_Store_Wires(vector<AaExpression*>* address_expressions,
				 vector<int>* scale_factors,
				 vector<int>* shift_factors,
				 ostream& ofile);


  void Write_VC_Load_Data_Path(vector<AaExpression*>* address_expressions,
			       vector<int>* scale_factors,
			       vector<int>* shift_factors,
			       AaExpression* target,
			       ostream& ofile);

  void Write_VC_Store_Data_Path(vector<AaExpression*>* address_expressions,
				vector<int>* scale_factors,
				vector<int>* shift_factors,
				AaExpression* source,
				ostream& ofile);

  void Write_VC_Load_Links(string hier_id,
			   vector<AaExpression*>* address_expressions,
			   vector<int>* scale_factors,
			   vector<int>* shift_factors,
			   ostream& ofile);

  void Write_VC_Store_Links(string hier_id,
			    vector<AaExpression*>* address_expressions,
			    vector<int>* scale_factors,
			    vector<int>* shift_factors,
			    ostream& ofile);


  void Write_VC_Load_Store_Control_Path(vector<AaExpression*>* address_expressions,
					vector<int>* scale_factors,
					vector<int>* shift_factors,
					string read_or_write,
					ostream& ofile);


  void Write_VC_Load_Store_Data_Path(vector<AaExpression*>* address_expressions,
				     vector<int>* scale_factors,
				     vector<int>* shift_factors,
				     AaExpression* data_expression,
				     string read_or_write,  
				     ostream& ofile);

  void Write_VC_Load_Store_Links( string hier_id,
				  string read_or_write,
				  vector<AaExpression*>* address_expressions,
				  vector<int>* scale_factors,
				  vector<int>* shift_factors,
				  ostream& ofile);



  void Write_VC_Address_Calculation_Constants(vector<AaExpression*>* address_expressions,
					      vector<int>* scale_factors,
					      vector<int>* shift_factors,
					      ostream& ofile);
  void Write_VC_Address_Calculation_Wires(vector<AaExpression*>* address_expressions,
					  vector<int>* scale_factors,
					  vector<int>* shift_factors,
					  ostream& ofile);
  void Write_VC_Address_Calculation_Control_Path(vector<AaExpression*>* address_expressions,
						 vector<int>* scale_factors,
						 vector<int>* shift_factors,
						 ostream& ofile);
  void Write_VC_Address_Calculation_Data_Path(vector<AaExpression*>* address_expressions,
					      vector<int>* scale_factors,
					      vector<int>* shift_factors,
					      ostream& ofile);


  void Write_VC_Root_Address_Calculation_Constants(vector<AaExpression*>* indices,
						   vector<int>* scale_factors,
						   vector<int>* shift_factors,
						   ostream& ofile);
  void Write_VC_Root_Address_Calculation_Wires(vector<AaExpression*>* indices,
					       vector<int>* scale_factors,
					       vector<int>* shift_factors,
					       ostream& ofile);

  void Write_VC_Address_Calculation_Links(string hier_id,
					  vector<AaExpression*>* address_expressions,
					  vector<int>* scale_factors,
					  vector<int>* shift_factors,
					  ostream& ofile);


  void Write_VC_Root_Address_Calculation_Control_Path(vector<AaExpression*>* indices,
						      vector<int>* scale_factors,
						      vector<int>* shift_factors,
						      ostream& ofile);
  void Write_VC_Root_Address_Calculation_Links(string hier_id,
					       vector<AaExpression*>* indices,
					       vector<int>* scale_factors,
					       vector<int>* shift_factors,
					       ostream& ofile);



  void Write_VC_Root_Address_Calculation_Data_Path(vector<AaExpression*>* indices,
						   vector<int>* scale_factors,
						   vector<int>* shift_factors,
						   ostream& ofile);


  virtual string Get_VC_Memory_Space_Name();
  virtual int Get_VC_Memory_Space_Index();

  virtual int Get_Base_Address();
  virtual int Get_Address_Width();
  virtual int Get_Word_Size();


  virtual string Get_VC_Base_Address_Name();
  virtual string Get_VC_Word_Address_Name(int idx);

  virtual string Get_VC_Offset_Name();
  virtual string Get_VC_Offset_Constant_Part_Name();
  virtual string Get_VC_Root_Address_Name();
  virtual string Get_VC_Word_Offset_Name(int idx);
  virtual string Get_VC_Offset_Scale_Factor_Name(int idx);
  virtual AaType* Get_Base_Address_Type();
  virtual int Get_Access_Width();
  virtual void Assign_Expression_Value(AaValue* expr_value);


  virtual bool Is_Load();
  virtual bool Is_Store();



  // control-path optimizations.
  void Write_VC_Load_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					    map<string, vector<AaExpression*> >& ls_map,
					    map<string,vector<AaExpression*> >& pipe_map,
					    vector<AaExpression*>* address_expressions,
					    vector<int>* scale_factors,
					    vector<int>* shift_factors,
					    ostream& ofile);
  
  void Write_VC_Store_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					     map<string, vector<AaExpression*> >& ls_map, 
					     map<string,vector<AaExpression*> >& pipe_map,
					     vector<AaExpression*>* address_expressions,
					     vector<int>* scale_factors,
					    vector<int>* shift_factors,
					     ostream& ofile);

  void Write_VC_Load_Store_Control_Path_Optimized(set<AaRoot*>& visited_elements,
						  map<string, vector<AaExpression*> >& ls_map, 
						  map<string,vector<AaExpression*> >& pipe_map,
						  vector<AaExpression*>* address_expressions,
						  vector<int>* scale_factors,
					    vector<int>* shift_factors,
						  string read_or_write,
						  ostream& ofile);
  
  void Write_VC_Load_Links_Optimized(string hier_id,
				     vector<AaExpression*>* address_expressions,
				     vector<int>* scale_factors,
					    vector<int>* shift_factors,
				     ostream& ofile);
  
  void Write_VC_Store_Links_Optimized(string hier_id,
				      vector<AaExpression*>* address_expressions,
				      vector<int>* scale_factors,
					    vector<int>* shift_factors,
				      ostream& ofile);


  void Write_VC_Load_Store_Links_Optimized( string hier_id,
					    string read_or_write,
					    vector<AaExpression*>* address_expressions,
					    vector<int>* scale_factors,
					    vector<int>* shift_factors,
					    ostream& ofile);

  void Write_VC_Address_Calculation_Control_Path_Optimized(set<AaRoot*>& visited_elements,
							   map<string,vector<AaExpression*> >& ls_map,
							   map<string,vector<AaExpression*> >& pipe_map,
							   vector<AaExpression*>* address_expressions,
							   vector<int>* scale_factors,
					    vector<int>* shift_factors,
							   ostream& ofile);
  void Write_VC_Address_Calculation_Links_Optimized(string hier_id,
						    vector<AaExpression*>* address_expressions,
						    vector<int>* scale_factors,
					    vector<int>* shift_factors,
						    ostream& ofile);


  void Write_VC_Root_Address_Calculation_Control_Path_Optimized(set<AaRoot*>& visited_elements,
								map<string,vector<AaExpression*> >& ls_map,
								map<string,vector<AaExpression*> >& pipe_map,
								vector<AaExpression*>* indices,
								vector<int>* scale_factors,
					    vector<int>* shift_factors,
								ostream& ofile);
  void Write_VC_Root_Address_Calculation_Links_Optimized(string hier_id,
							 vector<AaExpression*>* indices,
							 vector<int>* scale_factors,
					    vector<int>* shift_factors,
							 ostream& ofile);

  virtual bool Scalar_Types_Only() { return(false);}
  bool Get_Is_Dereferenced() {return(_is_dereferenced);}
  void Set_Is_Dereferenced(bool v);
};

// simple reference to a constant string (must be integer or real scalar or array)
class AaConstantLiteralReference: public AaObjectReference
{
  vector<string> _literals;
  bool _scalar_flag;
 public:
  AaConstantLiteralReference(AaScope* scope_tpr, 
			     string literal_string,  
			     vector<string>& literals);
  ~AaConstantLiteralReference();
  vector<string>& Get_Literals() {return(_literals);}
  void Set_Scalar_Flag(bool v) {_scalar_flag = v;}
  bool Get_Scalar_Flag() {return(_scalar_flag);}

  virtual string Kind() {return("AaConstantLiteralReference");}
  virtual void Map_Source_References(set<AaRoot*>& source_objects) {} // do nothing
  virtual void PrintC(ofstream& ofile, string tab_string);
  virtual void Write_VC_Control_Path( ostream& ofile);


  virtual void Get_Leaf_Expression_Set(set<AaExpression*>& leaf_expression_set)
  {
    leaf_expression_set.insert(this);
  }

  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Evaluate();

};

// simple reference (no array indices)
class AaSimpleObjectReference: public AaObjectReference
{

  set<AaStorageObject*> _addressed_objects;

 public:
  AaSimpleObjectReference(AaScope* scope_tpr, string object_ref_string);
  ~AaSimpleObjectReference();
  virtual string Kind() {return("AaSimpleObjectReference");}
  virtual void Set_Object(AaRoot* obj);
  virtual void Print(ostream& ofile);
  virtual void PrintC(ofstream& ofile, string tab_string);
  virtual void PrintC_Header_Entry(ofstream& ofile);

  virtual bool Set_Addressed_Object_Representative(AaStorageObject* obj);

  virtual void Get_Leaf_Expression_Set(set<AaExpression*>& leaf_expression_set)
  {
    leaf_expression_set.insert(this);
  }


  virtual bool Is_Implicit_Variable_Reference();
  virtual AaRoot* Get_Root_Object();
  virtual bool Is_Implicit_Object();

  virtual void Set_Type(AaType* t);
  virtual void Evaluate();

  virtual string Get_VC_Constant_Name();
  virtual string Get_VC_Driver_Name();
  virtual string Get_VC_Receiver_Name();
  virtual void Write_VC_Control_Path( ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target( ostream& ofile);
  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile);
  virtual void Write_VC_Wire_Declarations_As_Target(ostream& ofile);
  virtual void Write_VC_Datapath_Instances_As_Target( ostream& ofile, AaExpression* source);
  virtual void Write_VC_Datapath_Instances(AaExpression* target, ostream& ofile);
  virtual void Write_VC_Links(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target(string hier_id, ostream& ofile);

  string Get_VC_Name() {return("simple_obj_ref_" + Int64ToStr(this->Get_Index()));}
  virtual void Print_AddressOf_C(ofstream& ofile, string tab_string);
  virtual void Print_BaseStructRef_C(ofstream& ofile, string tab_string);

  virtual void Update_Type();

  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile);

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile);


  set<AaStorageObject*>& Get_Addressed_Objects()
    {
      return(this->_addressed_objects);
    }




};



// array object reference
class AaArrayObjectReference: public AaObjectReference
{

  // indices will in general be expressions
  vector<AaExpression*> _indices;
  AaSimpleObjectReference* _pointer_ref;

 public:

  vector<AaExpression*>* Get_Index_Vector() {return(&(_indices));}


  unsigned int Get_Number_Of_Indices() {return this->_indices.size();}

  // note: base object and all elements of index_list become children
  AaArrayObjectReference(AaScope* scope_tpr,
			 string object_ref_string, 
			 vector<AaExpression*>& index_list);
  ~AaArrayObjectReference();

  AaExpression* Get_Indices(int idx)
  {
    if(idx >= 0 && idx < _indices.size())
      return(_indices[idx]);
    else
      return(NULL);
  }
  virtual void Print(ostream& ofile); 
  AaExpression* Get_Array_Index(unsigned int idx);
  virtual void Set_Object(AaRoot* obj); 


  virtual string Kind() {return("AaArrayObjectReference");}
  virtual void Map_Source_References(set<AaRoot*>& source_objects); // important
  virtual void Map_Source_References_As_Target(set<AaRoot*>& source_objects);

  virtual void PrintC(ofstream& ofile, string tab_string);

  void Update_Address_Scaling_Factors(vector<int>& scale_factors,int word_size);
  void Update_Address_Shift_Factors(vector<int>& shift_factors,int word_size);

  virtual void Write_VC_Control_Path( ostream& ofile);
  void Write_VC_Address_Gen_Control_Path(ostream& ofile);

  virtual void Get_Leaf_Expression_Set(set<AaExpression*>& leaf_expression_set)
  {
    for(int idx = 0; idx < _indices.size(); idx++)
      _indices[idx]->Get_Leaf_Expression_Set(leaf_expression_set);
  }

  virtual void Write_VC_Control_Path_As_Target( ostream& ofile);
  virtual void Evaluate();

  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile);
  virtual void Write_VC_Wire_Declarations_As_Target(ostream& ofile);
  virtual void Write_VC_Datapath_Instances_As_Target( ostream& ofile, AaExpression* source);
  virtual void Write_VC_Datapath_Instances(AaExpression* target, ostream& ofile);
  virtual void Write_VC_Links(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target(string hier_id, ostream& ofile);
  string Get_VC_Name() {return("array_obj_ref_" + Int64ToStr(this->Get_Index()));}
  virtual void Print_AddressOf_C(ofstream& ofile, string tab_string);
  virtual void Print_BaseStructRef_C(ofstream& ofile, string tab_string);
  void Print_Indexed_C(AaType* t, int start_id, vector<AaExpression*>* indices, ofstream& ofile);

  virtual void Set_Type(AaType* t);
  virtual void Update_Type();


  virtual int Get_Base_Address();
  virtual string Get_VC_Base_Address_Name();
  virtual int Get_Address_Width();
  virtual int Get_Word_Size();



  virtual int Get_Access_Width();


  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile);

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile);

  virtual bool Set_Addressed_Object_Representative(AaStorageObject* obj);

  virtual void Add_Target_Reference(AaRoot* referrer); 
  virtual void Add_Source_Reference(AaRoot* referrer);
};


class AaPointerDereferenceExpression: public AaObjectReference
{
  AaObjectReference* _reference_to_object;
  set<AaStorageObject*> _addressed_objects_from_rhs;



 public:

  AaPointerDereferenceExpression(AaScope* scope, AaObjectReference* obj_ref);

  AaObjectReference* Get_Reference_To_Object()
  {
    return(_reference_to_object);
  }

  virtual void Map_Source_References_As_Target(set<AaRoot*>& source_objects)
  {
    this->_reference_to_object->Map_Source_References(source_objects);
  }

  virtual bool Is_Load() {return(!this->Get_Is_Target());}
  virtual bool Is_Store(){return(this->Get_Is_Target()); }
  virtual bool Is_Foreign_Store();
  virtual bool Is_Foreign_Load();

  virtual void Print(ostream& ofile);
  virtual void PrintC(ofstream& ofile, string tab_string);

  virtual void Evaluate();
  virtual void Propagate_Addressed_Object_Representative(AaStorageObject* obj, AaRoot* from_expr);

  virtual void Map_Source_References(set<AaRoot*>& source_objects); // important
  virtual void Update_Type();
  virtual string Kind() {return("AaPointerDereferenceExpression");}

  virtual void Write_VC_Control_Path( ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target( ostream& ofile);
  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile);
  virtual void Write_VC_Wire_Declarations_As_Target(ostream& ofile);
  virtual void Write_VC_Datapath_Instances_As_Target( ostream& ofile, AaExpression* source);
  virtual void Write_VC_Datapath_Instances(AaExpression* target, ostream& ofile);
  virtual void Write_VC_Links(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target(string hier_id, ostream& ofile);

  virtual string Get_VC_Name() {return("ptr_deref_" + Int64ToStr(this->Get_Index()));}

  virtual string Get_VC_Memory_Space_Name();
  virtual int Get_VC_Memory_Space_Index();

  virtual int Get_Base_Address();
  virtual int Get_Word_Size();
  virtual int Get_Address_Width();
  virtual AaType* Get_Base_Address_Type();
  virtual string Get_VC_Base_Address_Name();

  virtual int Get_Access_Width();
  virtual void Assign_Expression_Value(AaValue* expr_value)
  {
    // cannot assign a fixed value to a stored 
    // object..
  }

  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile);

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile);

  void Get_Siblings(set<AaPointerDereferenceExpression*>& sibling_set, bool get_sources, bool get_targets);

};


class AaAddressOfExpression: public AaObjectReference
{
  AaObjectReference* _reference_to_object;
  AaStorageObject* _storage_object;

 public:
  AaAddressOfExpression(AaScope* scope, AaObjectReference* obj_ref);

  virtual void Print(ostream& ofile);
  virtual void PrintC(ofstream& ofile, string tab_string);

  virtual bool Is_Load() {return(false);}
  virtual bool Is_Store(){return(false);}

  virtual void Propagate_Addressed_Object_Representative(AaStorageObject* obj, AaRoot* from_expr);

  virtual void Map_Source_References(set<AaRoot*>& source_objects); // important
  virtual void Update_Type();

  virtual string Kind() {return("AaAddressOfExpression");}

  virtual void Write_VC_Control_Path( ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target( ostream& ofile);
  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile);
  virtual void Write_VC_Wire_Declarations_As_Target(ostream& ofile);
  virtual void Write_VC_Datapath_Instances_As_Target( ostream& ofile, AaExpression* source);
  virtual void Write_VC_Datapath_Instances(AaExpression* target, ostream& ofile);
  virtual void Write_VC_Links(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target(string hier_id, ostream& ofile);
  string Get_VC_Name() {return("addr_of_" + Int64ToStr(this->Get_Index()));}

  virtual void Evaluate();

  virtual string Get_VC_Memory_Space_Name();
  virtual int Get_VC_Memory_Space_Index();

  virtual int Get_Base_Address();
  virtual int Get_Word_Size();
  virtual int Get_Address_Width();
  virtual AaType* Get_Base_Address_Type();
  virtual string Get_VC_Base_Address_Name();

  virtual int Get_Access_Width();

  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile);

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile);

  virtual bool Scalar_Types_Only() { return(true);}


};


// type cast expression (is unary)
class AaTypeCastExpression: public AaExpression
{
  AaType* _to_type;
  AaExpression* _rest;
  bool _bit_cast;
 public:

  AaType* Get_To_Type() {return(this->_to_type);}
  AaExpression* Get_Rest() {return(this->_rest);}

  AaTypeCastExpression(AaScope* scope, AaType* ref_type, AaExpression *rest);
  ~AaTypeCastExpression();

  void Set_Bit_Cast(bool v) { _bit_cast = v;}

  void Print(ostream& ofile);
  virtual string Kind() {return("AaTypeCastExpression");}
  virtual void Map_Source_References(set<AaRoot*>& source_objects) 
  {
    if(this->_rest) 
      this->_rest->Map_Source_References(source_objects);
  }
  virtual void PrintC(ofstream& ofile, string tab_string);

  virtual void Write_VC_Control_Path( ostream& ofile);
  virtual void Get_Leaf_Expression_Set(set<AaExpression*>& leaf_expression_set)
  {
    _rest->Get_Leaf_Expression_Set(leaf_expression_set);
  }

  virtual void Evaluate();

  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile);
  virtual void Write_VC_Datapath_Instances(AaExpression* target, ostream& ofile);
  virtual void Write_VC_Links(string hier_id, ostream& ofile);
  string Get_VC_Name() {return("type_cast_" + Int64ToStr(this->Get_Index()));}

  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile);

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile);



};



//
// unary expressions
// not B, <type-name> B
//
class AaUnaryExpression: public AaExpression
{
  AaOperation _operation;
  AaExpression* _rest;
 public:
  AaUnaryExpression(AaScope* scope_tpr, AaOperation operation, AaExpression* rest);
  ~AaUnaryExpression();
  virtual void Print(ostream& ofile); 

  AaOperation Get_Operation() {return(this->_operation);}
  AaExpression* Get_Rest() {return(this->_rest);}
  virtual string Kind() {return("AaUnaryExpression");}
  virtual void Map_Source_References(set<AaRoot*>& source_objects) 
  {
    if(this->_rest) 
      this->_rest->Map_Source_References(source_objects);
  }
  virtual void PrintC(ofstream& ofile, string tab_string)
  {
    ofile << tab_string << C_Name(this->_operation) << "(";
    this->_rest->PrintC(ofile,"");
    ofile << ") ";
  }

  virtual void Write_VC_Control_Path( ostream& ofile);
  virtual void Get_Leaf_Expression_Set(set<AaExpression*>& leaf_expression_set)
  {
    _rest->Get_Leaf_Expression_Set(leaf_expression_set);
  }


  virtual void Evaluate();
  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile);
  virtual void Write_VC_Datapath_Instances(AaExpression* target, ostream& ofile);
  virtual void Write_VC_Links(string hier_id, ostream& ofile);

  string Get_VC_Name() {return("unary_" + Int64ToStr(this->Get_Index()));}

  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile);

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile);


};

// 
// binary expression: q + r
//
class AaBinaryExpression: public AaExpression
{
  AaOperation _operation;
  AaExpression* _first;
  AaExpression* _second;

 public:
  AaBinaryExpression(AaScope* scope_tpr, AaOperation operation, AaExpression* first, AaExpression* second);
  ~AaBinaryExpression();
  virtual void Print(ostream& ofile);

  AaOperation Get_Operation() {return(this->_operation);}
  AaExpression* Get_First() {return(this->_first);}
  AaExpression* Get_Second() {return(this->_second);}
  virtual string Kind() {return("AaBinaryExpression");}
  virtual void Map_Source_References(set<AaRoot*>& source_objects) 
  {
    if(this->_first) this->_first->Map_Source_References(source_objects);
    if(this->_second) this->_second->Map_Source_References(source_objects);
  }
  virtual void PrintC(ofstream& ofile, string tab_string)
  {
    if(!Is_Concat_Operation(this->_operation))
      {
	ofile << tab_string 
	      << C_Name(this->_operation) << "(";
	this->_first->PrintC(ofile,"");
	ofile << ", ";
	this->_second->PrintC(ofile,"");
	ofile << ")" ;
      }
    else
      {
	int a_width = ((AaUintType*)(this->Get_Type()))->Get_Width();
	int c_width;
	if(a_width <= 8)
	  c_width = 8;
	else if(a_width <= 16)
	  c_width = 16;
	else if(a_width <= 32)
	  c_width = 32;
	else if(a_width <= 64)
	  c_width = 64;
	else
	  assert(0 && "maximum supported length of integer for Aa2C is 64");

	ofile << tab_string 
	      << C_Name(this->_operation) << "(" << c_width << "_t,";
	this->_first->PrintC(ofile,"");
	
	ofile << ", ";
	ofile << ((AaUintType*)this->_first->Get_Type())->Get_Width();
	ofile << ", ";
	this->_second->PrintC(ofile,"");
	ofile << ", ";
	ofile << ((AaUintType*)this->_first->Get_Type())->Get_Width();
	ofile << ")" ;
      }
  }

  virtual void Update_Type();


  virtual void Write_VC_Control_Path( ostream& ofile);

  virtual void Get_Leaf_Expression_Set(set<AaExpression*>& leaf_expression_set)
  {
    _first->Get_Leaf_Expression_Set(leaf_expression_set);
    _second->Get_Leaf_Expression_Set(leaf_expression_set);
  }

  bool Is_Trivial();
  virtual void Evaluate();
  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile);
  virtual void Write_VC_Datapath_Instances(AaExpression* target, ostream& ofile);
  virtual void Write_VC_Links(string hier_id, ostream& ofile);

  string Get_VC_Name() {return("binary_" + Int64ToStr(this->Get_Index()));}

  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile);

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile);



};

// ternary expression: a ? b : c
class AaTernaryExpression: public AaExpression
{
  AaExpression* _test;
  AaExpression* _if_true;
  AaExpression* _if_false;
 public:
  AaTernaryExpression(AaScope* scope_tpr, AaExpression* test,AaExpression* iftrue, AaExpression* iffalse);
  ~AaTernaryExpression();

  virtual void Print(ostream& ofile); 

  AaExpression*      Get_Test() {return(this->_test);}
  AaExpression* Get_If_True() {return(this->_if_true);}
  AaExpression* Get_If_False() {return(this->_if_false);}
  virtual string Kind() {return("AaTernaryExpression");}
  virtual void Map_Source_References(set<AaRoot*>& source_objects) 
  {
    if(this->_test) this->_test->Map_Source_References(source_objects);
    if(this->_if_true) this->_if_true->Map_Source_References(source_objects);
    if(this->_if_false) this->_if_false->Map_Source_References(source_objects);
  }

  virtual void PrintC(ofstream& ofile, string tab_string)
  {
    ofile << tab_string ;
    ofile << "( (";
    this->_test->PrintC(ofile,tab_string);
    ofile << ") ? (";
    this->_if_true->PrintC(ofile,tab_string);
    ofile << ") : (";
    this->_if_false->PrintC(ofile,tab_string);
    ofile << ") )";
  }

  virtual void Write_VC_Control_Path( ostream& ofile);
  virtual void Write_VC_Constant_Wire_Declarations(ostream& ofile);
  virtual void Write_VC_Wire_Declarations(bool skip_immediate, ostream& ofile);
  virtual void Write_VC_Datapath_Instances(AaExpression* target, ostream& ofile);
  virtual void Write_VC_Links(string hier_id, ostream& ofile);

  virtual void Get_Leaf_Expression_Set(set<AaExpression*>& leaf_expression_set)
  {
    _test->Get_Leaf_Expression_Set(leaf_expression_set);
    _if_true->Get_Leaf_Expression_Set(leaf_expression_set);
    _if_false->Get_Leaf_Expression_Set(leaf_expression_set);
  }

  virtual void Evaluate();

  string Get_VC_Name() {return("ternary_" + Int64ToStr(this->Get_Index()));}

  virtual void Write_VC_Links_Optimized(string hier_id, ostream& ofile);
  virtual void Write_VC_Links_As_Target_Optimized(string hier_id, ostream& ofile);

  virtual void Write_VC_Control_Path_Optimized(set<AaRoot*>& visited_elements,
					       map<string,vector<AaExpression*> >& ls_map,
					       map<string,vector<AaExpression*> >& pipe_map,
					       ostream& ofile);
  virtual void Write_VC_Control_Path_As_Target_Optimized(set<AaRoot*>& visited_elements,
							 map<string,vector<AaExpression*> >& ls_map,
							 map<string,vector<AaExpression*> >& pipe_map,
							 ostream& ofile);



};


			       
				
#endif
