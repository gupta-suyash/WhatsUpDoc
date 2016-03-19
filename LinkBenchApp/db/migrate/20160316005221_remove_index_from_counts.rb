class RemoveIndexFromCounts < ActiveRecord::Migration
  def change
	remove_index :counts, [:id, :link_type]
  end
end
