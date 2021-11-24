#pragma once


namespace text_format_rtf
{


   class std_output :
      virtual public output
   {

   public:


      ::std::ostream & m_output;


      std_output(::std::ostream & ostream);

      virtual void write(const void *, int len) override;



   };



} // namespace text_format_rtf




